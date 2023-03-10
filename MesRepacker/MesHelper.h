#pragma once

struct mescmd {
	int pos;
	int ulen;
	byte key;
};

class MesHelper{
protected:
	ReadBuffer* readbuffer;
	std::vector<mesconf*> mesconfs;
	std::vector<mescmd> cmds;
	mesconf* conf;
	uint16_t head_t;
	bool atsel;
	int offset;
	long size;

	void init() {
		if(atsel) this->conf = nullptr;
		this->offset = 0;
		this->head_t = 0;
		this->size = 0;
		cmds.clear();
	}

	short getHeadt(int offset) {
		byte head[2];
		this->readbuffer->get(head, offset, 2);
		return (head[0] << 8) | head[1];
	}

	bool selectConf() {
		if (!this->mesconfs.size() && this->conf) 
			return (this->offset = this->conf->offset(this->offset));
		uint16_t head_t = -1;
		int offset = -1;
		for (auto iter = this->mesconfs.begin(); iter != this->mesconfs.end(); iter++)
			if ((head_t = this->getHeadt(offset = (*iter)->offset(this->offset))) ==
				(*iter)->head_t && (this->conf = *iter)) break;
		if (this->conf) {
			this->type_name = this->conf->name;
			return (this->head_t = head_t) && (this->offset = offset);
		} else return false;
	}

	bool analyzing() {
		int size = this->readbuffer->fsize();
		int pos = this->offset + 3;
		while (pos < size) {
			mescmd cmd = mescmd();
			cmd.pos = pos;
			cmd.key = this->readbuffer->get(pos);
			if (pos >= 2088) {
				std::cout << "";
			}
			byte tmp = 0;
			if (this->conf->uint8x2.with(cmd.key)) {
				cmd.ulen = 3;
			}
			else if (this->conf->uint8str.with(cmd.key)) {
				cmd.ulen = 2;
				this->readbuffer->offset(pos + cmd.ulen);
				do {
					this->readbuffer->next(tmp);
					cmd.ulen++;
				} while (tmp);
			}
			else if (this->conf->str.with(cmd.key) || this->conf->decstr.with(cmd.key)) {
				cmd.ulen = 0;
				this->readbuffer->offset(pos);
				do {
					this->readbuffer->next(tmp);
					cmd.ulen++;
				} while (tmp);
			}
			else if (this->conf->shortx4.with(cmd.key)) {
				cmd.ulen = 9;
			}
			else {
				return false;
			}
			pos += cmd.ulen;
			this->cmds.push_back(cmd);
		}
		return true;
	}

	MesHelper() {
		this->readbuffer = nullptr;
		this->atsel = true;
		this->init();
	}

	MesHelper(std::vector<mesconf*> mesconfs) : MesHelper() {
		this->mesconfs = mesconfs;
	}


	MesHelper(mesconf* conf) : MesHelper() {
		this->atsel = false;
		this->conf = conf;
		this->type_name = this->conf->name;
	}

	void destroy() {
		for (auto iter = this->mesconfs.begin(); iter != this->mesconfs.end(); iter++)
			delete* iter;
		this->mesconfs.clear();
		this->cmds.clear();
		delete this->readbuffer;
	}

	void setFileName(std::string filepath) {
		filepath.assign(filepath.substr(filepath.find_last_of("\\") + 1));
		this->filename.assign(filepath.substr(0, filepath.find_last_of(".")));
	}

	bool load(std::string filepath) {
		this->init();
		if (!this->readbuffer) this->readbuffer = new ReadBuffer();
		if (this->readbuffer->reader(filepath)->hasData()) {
			byte tmp[4] = { 0 };
			if (this->readbuffer->get(tmp, 0, 4)) {
				this->offset = (tmp[3] << 24) | (tmp[2] << 16) | (tmp[1] << 8) | tmp[0];
				return selectConf() && analyzing();
			}
		}
		return false;
	}

public:
	std::string filename;
	std::string type_name;
};


class MesTextHelper: public MesHelper {

	void output(FILE* fp, std::string& str) {
		byte tmp;
		for (size_t i = 0; i < str.size(); i++) {
			tmp = str.c_str()[i];
			if (tmp == '\n') fputs("\\n", fp);
			else fputc(tmp, fp);
		}
		fputc('\r\n', fp);
	}

	void textOutput(FILE* fp, std::string& str, int pos, int count) {
		if (is_not_strcon) {
			fprintf(fp, "#0x%x:", pos);
			output(fp, str);
		}
		else {
			fprintf(fp, "#0x%x\n", pos);
			std::string mark("??????  " + std::to_string(count) + "  ??????");
			//mark.assign(gbk2utf8((char*)mark.c_str()));
			output(fp, std::string(str).insert(0, std::string(mark).append("//")));
			output(fp, std::string(str).insert(0, std::string(mark)));
			fputc('\r\n', fp);
		}
	}

	bool is_igbk;
	bool is_not_strcon;

public:
	MesTextHelper(std::vector<mesconf*> mesconfs, bool is_not_strcon, bool is_igbk) : MesHelper(mesconfs) {
		this->is_not_strcon = is_not_strcon;
		this->is_igbk = is_igbk;
	}
	MesTextHelper(mesconf* conf, bool is_not_strcon, bool is_igbk) : MesHelper(conf) {
		this->is_not_strcon = is_not_strcon;
		this->is_igbk = is_igbk;
	}

	void outTextToFile(std::string outpath) {
		FILE* out = fopen(outpath.append(this->filename + ".txt").c_str(), "w");
		std::string outstr;
		int count = 0;
		byte* tmp = { 0 };
		//this->is_igbk = true;
		for (auto iter = this->cmds.begin(); iter != this->cmds.end(); iter++) {
			if (this->conf->decstr.with((*iter).key)) {
				//continue;
				tmp = new byte[(*iter).ulen];
				this->readbuffer->get(tmp, (*iter).pos + 1, (*iter).ulen - 1);
				for (int i = 0; i < (*iter).ulen - 2; i++) tmp[i] += 0x20;
				tmp[(*iter).ulen - 1] = '\0';
				outstr.assign(this->is_not_strcon ? (char*)tmp : this->is_igbk ? gbk2utf8((char*)tmp) : sj2utf8((char*)tmp));
				//printf("key: 0x%x pos: %d str: %s\n", (int)(*iter).key, (*iter).pos, outstr.c_str());
				textOutput(out, outstr, (*iter).pos, ++count);
				delete[] tmp;
			}
			else if (this->conf->optundec && this->conf->optundec == (*iter).key) {
				//continue;
				tmp = new byte[(*iter).ulen];
				this->readbuffer->get(tmp, (*iter).pos + 1, (*iter).ulen - 1);
				tmp[(*iter).ulen - 1] = '\0';
				outstr.assign(this->is_not_strcon ? (char*)tmp : this->is_igbk ? gbk2utf8((char*)tmp) : sj2utf8((char*)tmp));
				//printf("key: 0x%x pos: %d str: %s\n", (int)(*iter).key, (*iter).pos, outstr.c_str());
				textOutput(out, outstr, (*iter).pos, ++count);
				delete[] tmp;
			}
			if (this->conf->str.with((*iter).key)) { // test opt
				continue;
				tmp = new byte[(*iter).ulen];
				this->readbuffer->get(tmp, (*iter).pos + 1, (*iter).ulen - 1);
				tmp[(*iter).ulen - 1] = '\0';
				outstr.assign(this->is_igbk ? gbk2utf8((char*)tmp) : sj2utf8((char*)tmp));
				printf("key: 0x%x pos: %d str: %s\n", (int)(*iter).key, (*iter).pos, outstr.c_str());
			}
		}
		fclose(out);
	}

	bool load(std::string filepath) {
		this->setFileName(filepath);
		return MesHelper::load(filepath);
	}

	void optInfo() {
		std::cout
			<< "file name: " << filename << std::endl
			<< "file type: " << conf->name << std::endl
			<< "size: " << size << std::endl
			<< "head_t: 0x" << std::hex << head_t << std::endl;
	}

	void destroy() {
		MesHelper::destroy();
	}

};

class MesRepacker : public MesHelper {

	std::string mespath;
	TextReadBuffer* textReadBuffer;
	WriteBuffer* writeBuffer;
	bool is_not_strcon;

public:
	MesRepacker(std::vector<mesconf*> mesconfs, std::string mespath, bool is_not_strcon) : MesHelper(mesconfs) {
		this->mespath = mespath;
		this->is_not_strcon = is_not_strcon;
		if (!this->textReadBuffer) this->textReadBuffer = new TextReadBuffer();
	}
	MesRepacker(mesconf* conf, std::string mespath, bool is_not_strcon) : MesHelper(conf) {
		this->mespath = mespath;
		this->is_not_strcon = is_not_strcon;
		if (!this->textReadBuffer) this->textReadBuffer = new TextReadBuffer();
	}

	bool load(std::string filepath) {
		this->setFileName(filepath);
		std::string mesfile(this->mespath + this->filename + ".mes");
		if (!std::filesystem::exists(mesfile.c_str())) return false;
		return this->textReadBuffer->reader(filepath, this->is_not_strcon)->hasData() 
			&& MesHelper::load(mesfile);
	}

	void outMesFile(std::string outpath) {
		if (!this->writeBuffer) this->writeBuffer = new WriteBuffer();
		this->writeBuffer->write(this->readbuffer, 0, this->offset + 3);
		TextMapHelper* textMaps = this->textReadBuffer->getTextMaps();
		int blockCurrent = 1;
		byte* tmp = { 0 };
		std::string text;
		int len = 0;
		for (auto iter = this->cmds.begin(); iter != this->cmds.end(); iter++) {
			if (textMaps->get((*iter).pos, text)) {
				len = strlen(text.c_str()) + 2;
				tmp = new byte[len];
				*tmp = (*iter).key;
				memcpy(tmp + 1, text.c_str(), strlen(text.c_str()) + 1);
				if (this->conf->decstr.with((*iter).key))
					for (int i = 1; i < len - 1; i++) tmp[i] -= 0x20;
				this->writeBuffer->write(tmp, len);
				delete[] tmp;
			}
			else {
				this->writeBuffer->write(this->readbuffer, (*iter).pos, (*iter).ulen);
			}
			if ((*iter).key == 0x3 || (*iter).key == 0x4) {
				len = this->writeBuffer->lensub(this->offset + 3);
				tmp = new byte[3];
				tmp[0] =  len & 0x000000ff;
				tmp[1] = (len & 0x0000ff00) >> 8;
				tmp[2] = (len & 0x00ff0000) >> 16;
				this->writeBuffer->rewrite(tmp, ++blockCurrent * 4, 3);
				delete[] tmp;
			}
		}
		this->writeBuffer->outFile(outpath.append(this->filename + ".mes").c_str());
		this->writeBuffer->reset();
	}

	void destroy() {
		if (this->textReadBuffer) delete this->textReadBuffer;
		if (this->writeBuffer) delete this->writeBuffer;
		MesHelper::destroy();
	}
};
