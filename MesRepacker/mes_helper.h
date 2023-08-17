#pragma once
namespace mes_helper {
	
	struct command {
		int32_t pos;
		uint8_t key;
		int32_t len;
	};
}

namespace mes_helper::text {

	using namespace StringHelper;
	const uint32_t max_length = 24;
	const uint32_t mim_length = 22;
	UTF8String begin_symbols(u8"¡£¡¢£¿¡¯¡±£¬£¡¡«¡¿£»£º£©¡¹¡»");
	UTF8String end_symbols  (u8"£¨(¡¸¡º¡¾¡®¡°¡­");

	void __text_before_clear(UTF8String& text) {
		text.trim()
			.remove(u8"\\n¡¡")
			.remove(u8"\\n")
			.replace(u8"/", u8"£¯")
			.replace(u8"{", u8"£û")
			.replace(u8"}", u8"£ý")
			.commit();
	}

	bool __is_talking(UTF8String& text) {
		return (text.start_with(u8"¡¸") && text.end_with(u8"¡¹")) ||
			   (text.start_with(u8"¡º") && text.end_with(u8"¡»"));
	}

	void formater(const char* str, UTF8String* out_result_ptr, bool escape) {
		if (!str || !out_result_ptr) return;
		utf8str text(str), tmpstr, result;
		__text_before_clear(text);
		uint32_t length = text.get_length();
		bool is_talking = __is_talking(text);
		bool go_continue = false;
		bool add_new_line = false;
		float add_w_count = 0;
		uint32_t index = 0;
		if (!length) return;
		while (index < length) {
			cchar tmp = text.at(index);
			if (add_new_line) {
				if (!begin_symbols.contains(tmp) && tmp != u8"\x20") {
				_add_new_line:
					result.append(escape ? u8"\\n" : u8"\n")
						.append(is_talking ? u8"¡¡" : NULL);
					add_w_count = is_talking;
					if (tmpstr.get_length()) {
						result.append(tmpstr);
						tmpstr.clear();
						continue;
					}
					else if (go_continue) {
						add_new_line = false;
						go_continue = false;
						continue;
					}
				}
			}
			if (tmp == u8"£û") {
				uint32_t center = text.find(u8"£¯", index);
				uint32_t end = text.find(u8"£ý", index + center);
				if (center != -1 && end != -1) {
					tmpstr = text.substrs(index, center + end + 1);
					index = index + center + end;
					add_w_count += (float)(end - 1.0f);
					if (add_w_count >= (float)max_length) {
						goto _add_new_line;
					}
					result.append(tmpstr);
					tmpstr.clear();
				}
				else {
					goto _append;
				}
			}
			else if (tmp.get_size() == 1) {
				if (tmp != u8"\x20" && index != length - 1) {
					uint32_t start = index, s_len = 1;
					while ((tmp = text.at(index + s_len++)).get_size() == 1 && tmp != u8"\x20");
					tmpstr = text.substrs(start, s_len - 1);
					index = index + s_len - 1;
					if (!tmpstr.contains(u8"@")) {
						add_w_count += (float)s_len * 0.5f;
						if (add_w_count >= (float)max_length) {
							goto _add_new_line;
						}
					}
					result.append(tmpstr);
					tmpstr.clear();
					goto _append;
				}
				else {
					add_w_count -= 0.5f;
					goto _append;
				}
			}
			else {
			_append:
				add_w_count++;
				result.append(tmp);
			}
			if (add_new_line = (add_w_count >= (float)mim_length)) {
				if (end_symbols.contains(tmp)) {
					go_continue = true;
					goto _add_new_line;
				}
			}
			index++;
		}
		out_result_ptr->assign(result);
	}
}

namespace mes_helper::text_map {
	
	using namespace configuration::repacker;
	std::map<int32_t, std::string> text_map;

	void init() {
		text_map.clear();
	}

	void text_converts(std::string& text) {
		using namespace charset_helper;
		if (!configuration::init::is_not_convert) {
			converts(text, CP_UTF8, use_code_page);
		}
	}

	void add(int32_t pos, std::string text) {
		text_map::text_converts(text);
		text_map.insert(std::make_pair(pos, text));
	}

	bool get(int32_t pos, std::string& text) {
		auto iter = text_map.find(pos);
		if (iter == text_map.end()) {
			return false;
		}
		else {
			text.assign((*iter).second);
			return true;
		}
	}
}

namespace mes_helper::instance {

	FileHelper::ReadBuffer readbuffer;
	std::vector<command>   cmd_array ;
	uint32_t offset = 0;

	bool select_config() {
		if (configuration::init::config) {
			using namespace configuration::init;
			offset = (*config).offset(offset) + 3;
			return true;
		} else if (!configuration::mes::configs.empty()) {
			using namespace configuration;
			for (mes_config& config : mes::configs) {
				int32_t h_offset = config.offset(offset);
				uint16_t head_t = readbuffer.get_int16(h_offset);
				*((uint8_t*)&head_t) ^= *(((uint8_t*)&head_t) + 1);
				*(((uint8_t*)&head_t) + 1) ^= *((uint8_t*)&head_t);
				*((uint8_t*)&head_t) ^= *(((uint8_t*)&head_t) + 1);
				if (config.head_t == head_t) {
					configuration::init::config = &config;
					instance::offset = h_offset + 3;
					return true;
				}
			}
		}
		return false;
	}

	void command_parsing() {
		instance::offset = (uint32_t)readbuffer.get_int32(0);
		if (select_config() && configuration::init::config) {
			using namespace configuration;
			mes_config& conf = (mes_config&)*init::config;
			uint8_t* buffer  = (uint8_t*)readbuffer.data();
			size_t bf_size   = readbuffer.size();
			uint32_t cur_pos = instance::offset;
			while (cur_pos < bf_size) {
				command n_cmd{ (int32_t)cur_pos, buffer[cur_pos], 0 };
				uint8_t data_tmp = 0, * buf_tmp = 0;
				if (conf.uint8x2.with(n_cmd.key)) {
					n_cmd.len = 3;
				}
				else if (conf.uint8str.with(n_cmd.key)) {
					n_cmd.len = 2;
					buf_tmp = buffer + cur_pos + 2;
					do {
						data_tmp = *buf_tmp++;
						n_cmd.len++;
					} while (data_tmp);
				}
				else if (conf.str.with(n_cmd.key) || conf.decstr.with(n_cmd.key)) {
					buf_tmp = buffer + cur_pos;
					do {
						data_tmp = *buf_tmp++;
						n_cmd.len++;
					} while (data_tmp);
				}
				else if (conf.shortx4.with(n_cmd.key)) {
					n_cmd.len = 9;
				}
				else throw std::exception("Script parsing error!");
				cur_pos = cur_pos + n_cmd.len;
				cmd_array.push_back(n_cmd);
			}
		}
		else throw std::exception("The mes file version is not supported!");
	}

	void load(std::string path) {
		instance::cmd_array.clear();
		readbuffer.reader(path.c_str());
	}

	void sbf_write(FileHelper::WriteBuffer& wr_buf, size_t bf_pos = 0, size_t bf_size = 0) {
		if (!bf_pos && !bf_size) {
			wr_buf.write((int8_t*)readbuffer.data(), 0, instance::offset);
		}
		else {
			wr_buf.write((int8_t*)readbuffer.data(), bf_pos, bf_size);
		}
	}
}

namespace mes_helper::loader {

	std::string text;
	std::filesystem::path cur_file;
	FileHelper::WriteBuffer wr_buf;
	bool is_import_text  = false;
	bool out_config_file = false;
	int32_t str_write_count = 0;

	void file_repacker() {
		cur_file = configuration::repacker::current_file;
		is_import_text = true;

	}

	void file_exporter() {
		is_import_text  = false;
		str_write_count = 0;
	}

	void mes_loading() {
		instance::load(cur_file.string());
		instance::command_parsing();
	}

	void write_clear() {
		wr_buf.clear();
	}

	void wirte_text_to_buffer(size_t pos, size_t count_num, sstr& str) {
		if(str.is_empty() || !str.get_c_strlen()) return;
		using namespace configuration::init;
		if (configuration::init::is_not_convert) {
			const char* bf_str = str.get_c_str();
			int32_t bf_size = strlen(bf_str) + 24;
			wr_buf.format_write("#0x%x:%s\n", bf_size, pos, bf_str);
		}
		else {
			using namespace charset_helper;
			converts(str, is_iput_gbk ? 936 : 932, 65001);
			const char* bf_str = str.get_c_str();
			int32_t bf_size = strlen(bf_str) + 24;
			wr_buf.format_write(u8"#0x%x\n", 24, pos);
			wr_buf.format_write(u8"¡ï¡ò  %d  ¡ò¡ï//%s\n", bf_size, count_num, bf_str);
			wr_buf.format_write(u8"¡ï¡ò  %d  ¡ò¡ï%s\n\n", bf_size, count_num, bf_str);
		}
	}

	void wirte_text(size_t pos, size_t count_num, sstr& str) {
		wirte_text_to_buffer(pos, count_num, str.replace("\n", "\\n"));
	}

	void export_text(size_t count = 1) {
		char* buffer = (char*)instance::readbuffer.data();
		for (command& cmd : instance::cmd_array) {
			if ((*configuration::init::config).decstr.with(cmd.key)) {
				sstr text(buffer + cmd.pos + 1);
				if (text.get_length()) {
					const char* bf_str = text.get_c_str();
					char* desstr = (char*)bf_str;
					do { *desstr++ += 0x20; } while (*desstr);
					wirte_text(cmd.pos, count++, text);
				}
				continue;
			}
			if (cmd.key != 0x0 && (*configuration::init::config).opt_undec == cmd.key) {
				sstr text(buffer + cmd.pos + 1);
				if (text.get_length()) {
					wirte_text(cmd.pos, count++, text);
				}
				continue;
			}
			if ((*configuration::init::config).str.with(cmd.key)) { // test
				continue;
				std::string str(buffer + cmd.pos + 1);
				std::cout << "key:" << std::hex << (int32_t)cmd.key << " " << str << std::endl;
			}
		}
	}

	void import_text() {
		int32_t block_num = 1;
		instance::sbf_write(wr_buf);
		for (command& cmd : instance::cmd_array) {
			if (text_map::get(cmd.pos, text)) {
				wr_buf.write_int8(cmd.key);
				int8_t* bfstr = (int8_t*)text.c_str();
				size_t length = strlen((char*) bfstr);
				if ((*configuration::init::config).decstr.with(cmd.key)) {
					int8_t* desstr = bfstr;
					do { *desstr++ -= 0x20; } while(*desstr);
				}
				wr_buf.write(bfstr, length + 1);
			}
			else {
				instance::sbf_write(wr_buf, cmd.pos, cmd.len);
			}
			if (cmd.key == 0x3 || cmd.key == 0x4) {
				int32_t block_pos = ++block_num * 4;
				int32_t block_fg  = (*(int32_t*)(wr_buf.data() + block_pos));
				int32_t bf_size   = (wr_buf.wbf_size() - instance::offset);
				block_fg = (block_fg & 0xff000000) | bf_size;
				wr_buf.rewrite_int32(block_fg, block_pos);
			}
		}
	}

	void create_config() {
		loader::write_clear();
		std::string input_path(std::filesystem::is_directory(cur_file)
			? cur_file.string() : cur_file.parent_path().string());
		char* bf_str = (char*)input_path.c_str();
		int32_t bf_size = strlen(bf_str) + 24;
		wr_buf.format_write("#InputPath\n%s\n\n", bf_size, bf_str);
		wr_buf.write_text("#UseCodePage\n936\n\n");
		wr_buf.write_text("#Before-Replaces\n[]:[]\n\n");
		wr_buf.write_text("#After-Replaces\n[]:[]\n");
		loader::out_config_file = true;
	}

	bool out_path_create(std::filesystem::path& path, const char* file_cat) {
		try {
			if (!std::filesystem::exists(path)) {
				std::filesystem::create_directory(path);
			}
			if (file_cat) {
				path /= cur_file.stem().string() + file_cat;
			}
			return true;
		}
		catch (const std::exception&) {
			return false;
		}
	}

	void out(std::filesystem::path path) {
		using namespace configuration::init;
		if (is_import_text) {
			path /= std::string((*config).name) + "_mes";
			if (out_path_create(path, ".mes")) goto _out;
		}
		else if(config) {
			path /= std::string((*config).name) + "_text";
			if (out_config_file) {
				out_config_file = false;
				if (std::filesystem::exists(path)) {
					path /= ".MesRepacker";
					goto _out;
				}
			}
			if (out_path_create(path, ".txt")) goto _out;
		} 
	_err:
		throw std::exception("Failed to create file!");
	_out:
		wr_buf.out_file(path.string().c_str());

	}
}