#pragma once

class ReadBuffer {
	byte* buffer;
	long size;
	int position;

	void init() {
		if (this->buffer) delete[] buffer;
		this->buffer = nullptr;
		this->position = 0;
		this->size = 0;
	}

	ReadBuffer* reader(FILE* file) {
		if (file) {
			fseek(file, 0, SEEK_END);
			this->size = ftell(file);
			if (!this->size) return this;
			this->buffer = new byte[this->size];
			fseek(file, 0, SEEK_SET);
			fread(this->buffer, 1, this->size, file);
			this->position = 0;
			fclose(file);
		}
		return this;
	}

public:
	ReadBuffer() {
		this->init();
	}

	~ReadBuffer() {
		this->init();
	}

	ReadBuffer* reader(std::string filepath) {
		this->init();
		return this->reader(fopen(filepath.c_str(), "rb"));
	}

	bool hasData() {
		return this->size && this->buffer;
	}

	void offset(int pos) {
		this->position = pos;
	}

	bool next(byte& bf) {
		if (this->position > this->size - 1)
			return false;
		bf = this->buffer[this->position++];
		return true;
	}

	bool get(byte* bf, int pos, int len) {
		if (pos > this->size || pos + len > this->size)
			return false;
		memcpy(bf, &this->buffer[pos], len);
		return true;
	}

	byte get(int pos) {
		return this->buffer[pos];
	}

	long fsize() {
		return this->size;
	}
};

class TextReadBuffer {

	TextMapHelper *textMapHelper;

	TextReadBuffer* reader(std::ifstream ifs, bool is_not_strcon) {
		if (!ifs.is_open()) return this;
		std::string line, text, pos;
		int codepage = 936, position = 0;
		while (std::getline(ifs, line))
			if (!is_not_strcon && line.find("#UseCodePage") != -1) {
				line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
				line.assign(line.substr(line.find_first_of(":") + 1));
				codepage = std::stoi(line);
			}
			else if (line.find("#0x") != -1) {
				pos = line.substr(0, line.find_first_of(":"));
				pos.assign(pos.substr(pos.find_first_of("#") + 1));
				pos.erase(std::remove_if(pos.begin(), pos.end(), isspace), pos.end());
				if (!sscanf(pos.c_str(), "%x", &position)) continue;
				text = line.substr(line.find_first_of(":") + 1);
				this->textMapHelper->push(position, is_not_strcon ? text : strcon(text, codepage));
			}
		ifs.close();
		return this;
	}
public:

	TextReadBuffer() {
		this->textMapHelper = nullptr;
		this->init();
	}

	~TextReadBuffer() {
		if (!this->textMapHelper) return;
		this->textMapHelper->clear();
		delete this->textMapHelper; 
	}

	void init() {
		if (!this->textMapHelper) this->textMapHelper = new TextMapHelper();
		this->textMapHelper->clear();
	}

	TextReadBuffer* reader(std::string filepath, bool is_not_strcon) {
		this->init();
		return this->reader(std::ifstream(filepath, std::ios::in), is_not_strcon);
	}

	bool hasData() {
		return textMapHelper->size();
	}

	TextMapHelper* getTextMaps() {
		return this->textMapHelper;
	}

};
