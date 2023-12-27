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
	uint32_t max_length = 24;
	uint32_t min_length = 22;
	UTF8String begin_symbols(u8"。、？’”，！～】；：）」』");
	UTF8String end_symbols  (u8"（(「『【‘“…");
	bool enable_formater = true;

	void config_init() {
		using namespace configuration::repacker;
		if (text_min_length == -1 || text_max_length == -1) {
			enable_formater = false;
		}
		else if (text_max_length > 0 && text_min_length > 0 && text_min_length <= text_max_length) {
			mes_helper::text::max_length = text_max_length;
			mes_helper::text::min_length = text_min_length;
		}
	}

	void __text_before_clear(UTF8String& text) {
		text.trim()
			.remove(u8"\\n　")
			.remove(u8"\\n")
			.replace(u8"/", u8"／")
			.replace(u8"{", u8"｛")
			.replace(u8"}", u8"｝")
			.commit();
	}

	bool __is_talking(UTF8String& text) {
		const char* start[] = { u8"「" , u8"『", u8"“" };
		const char* end[]   = { u8"」" , u8"』", u8"”" };
		for (uint8_t i = 0; i < 3; i++) {
			if (text.start_with(start[i]) && text.end_with(end[i])) {
				return true;
			}
		}
		return false;
	}

	void formater(const char* str, UTF8String* out_result_ptr, bool escape) {
		if (!enable_formater || !str || !out_result_ptr) return;
		utf8str text(str), tmpstr, result;
		__text_before_clear(text);
		uint32_t length   = text.get_length();
		bool is_talking   = __is_talking(text);
		bool go_continue  = false;
		bool add_new_line = false;
		float add_w_count = 0;
		uint32_t index = 0;
		if (!length) return;
		while (index < length) {
			cchar tmp = text.at(index);
			if (add_new_line) {
				if (!begin_symbols.contains(tmp) && tmp != u8"\x20") {
				_add_new_line:
					result.append(escape ? u8"\\n" : u8"\n");
					result.append(is_talking ? u8"　" : NULL);
					add_w_count = is_talking;
					if (tmpstr.get_length()) {
						result.append(tmpstr);
						tmpstr.clear();
						continue;
					}
					else if (go_continue) {
						add_new_line = false;
						go_continue  = false;
						continue;
					}
				}
			}
			if (tmp == u8"｛") {
				uint32_t center = text.find(u8"／", index);
				uint32_t end = text.find(u8"｝", index + center);
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
			if (add_new_line = (add_w_count >= (float)min_length)) {
				if (end_symbols.contains(tmp)) {
					go_continue = true;
					goto _add_new_line;
				}
			}
			index++;
		}
		(*out_result_ptr).assign(result);
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
		if (!text.empty() && text.size() > 0) {
			text_map::text_converts(text);
			text_map.insert(std::make_pair(pos, text));
		}
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

	uint32_t offset = 0;
	int32_t block_start;
	files::readbuffer   readbuffer;
	std::vector<command> cmd_array;

	bool select_config_if_exists() {
		uint16_t version = 0x00000;
		uint8_t* data = readbuffer.data();
		if (((uint32_t*)data)[1] == 0x3) {
			instance::offset = *((uint32_t*)data) * 6 + 4;
			version = *(uint16_t*)(data + instance::offset);
			instance::block_start = 1;
			instance::offset += 3;
		}
		else {
			instance::offset = *((uint32_t*)data) * 4 + 4;
			version = *(uint16_t*)(data + instance::offset);
			instance::block_start = 0;
			instance::offset += 2;
		}
		using namespace configuration;
		if (!init::config && !mes::configs.empty()) {
			for (mes_config& config : mes::configs) {
				if (config.version != version) continue;
				configuration::init::config = &config;
			}
		}
		return configuration::init::config;
	}

	void command_parsing() {
		if (!readbuffer.empty() && select_config_if_exists()) {
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
				else if (conf.string.with(n_cmd.key) || conf.decstr.with(n_cmd.key)) {
					buf_tmp = buffer + cur_pos;
					do {
						data_tmp = *buf_tmp++;
						n_cmd.len++;
					} while (data_tmp);
				}
				else if (conf.uint16x4.with(n_cmd.key)) {
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
		readbuffer.read(path.c_str());
	}

	void sbf_write(files::writebuffer& wr_buf, size_t bf_pos = 0, size_t bf_size = 0) {
		uint8_t* data = (uint8_t*)readbuffer.data() + bf_pos;
		wr_buf.write(data, bf_size != 0 ? bf_size : instance::offset);
	}
}

namespace mes_helper::loader {

	std::string text;
	files::writebuffer wr_buf;
	std::filesystem::path cur_file;
	bool is_import_text  = false;
	bool out_config_file = false;
	int32_t str_write_count = 0;

	void file_repacker() {
		cur_file = configuration::repacker::current_file;
		out_config_file = false;
		is_import_text  = true;
	}

	void file_exporter() {
		is_import_text  = false;
		out_config_file = false;
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
			converts(str, is_input_gbk ? 936 : 932, 65001);
			const char* bf_str = str.get_c_str();
			int32_t bf_size = strlen(bf_str) + 0x30;
			wr_buf.format_write(u8"#0x%X\n", 0x15, pos);
			wr_buf.format_write(u8"★◎  %03d  ◎★//%s\n", bf_size, count_num, bf_str);
			wr_buf.format_write(u8"★◎  %03d  ◎★%s\n\n", bf_size, count_num, bf_str);
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
					while (*desstr) { *desstr++ += 0x20; }
					wirte_text(cmd.pos, count++, text);
				}
				continue;
			}
			if (cmd.key != 0x0 && (*configuration::init::config).optundec == cmd.key) {
				sstr text(buffer + cmd.pos + 1);
				if (text.get_length()) {
					wirte_text(cmd.pos, count++, text);
				}
				continue;
			}
			if ((*configuration::init::config).string.with(cmd.key)) { // test
				continue;
				//std::string str(buffer + cmd.pos + 1);
				//std::cout << "key:" << std::hex << (int32_t)cmd.key << " " << str << std::endl;
			}
		}
	}

	void import_text() {
		instance::sbf_write(wr_buf);
		int32_t block_num = instance::block_start;
		for (command& cmd : instance::cmd_array) {
			if (text_map::get(cmd.pos, text)) {
				wr_buf.write(&cmd.key, 1);
				uint8_t* bfstr = (uint8_t*)text.c_str();
				size_t length = text.length();
				if ((*configuration::init::config).decstr.with(cmd.key)) {
					uint8_t* desstr = bfstr;
					while (*desstr) { *desstr++ -= 0x20; }
				}
				wr_buf.write(bfstr, length + 1);
			}
			else {
				instance::sbf_write(wr_buf, cmd.pos, cmd.len);
			}
			if (cmd.key == 0x3 || cmd.key == 0x4) {
				int32_t block_pos = ++block_num * 4;
				int32_t block_fg  = (*(int32_t*)(wr_buf.data() + block_pos));
				int32_t bf_size   = (wr_buf.size() - instance::offset);
				block_fg = (block_fg & 0xff000000) | bf_size;
				wr_buf.rewrite(block_pos, (int8_t*)&block_fg, 4);
			}
		}
	}

	void create_config() {
		loader::write_clear();
		std::string input_path(std::filesystem::is_directory(cur_file)
			? cur_file.string() : cur_file.parent_path().string());
		using namespace configuration::repacker::config;
		char* bf_str = (char*)input_path.c_str();
		int32_t bf_size = input_path.size() + strlen(path) + 24;
		wr_buf.format_write("%s\n%s\n\n", bf_size, path, bf_str);
		if (!configuration::init::is_not_convert) {
			wr_buf.format_write("%s\n%d\n\n", strlen(cdpg) + 24, cdpg, 936);
			wr_buf.format_write("%s\n%d\n\n", strlen(tmin) +  6, tmin,  22);
			wr_buf.format_write("%s\n%d\n\n", strlen(tmax) +  6, tmax,  24);
			wr_buf.format_write("%s\n[]:[]\n\n", strlen(bfrp) + 24, bfrp);
			wr_buf.format_write("%s\n[]:[]\n\n", strlen(atrp) + 24, atrp);
		}
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
		using namespace configuration;
		if (is_import_text) {
			path /= std::string((*init::config).name) + "_mes";
			if (out_path_create(path, ".mes")) goto _out;
		}
		else if(init::config) {
			path /= std::string((*init::config).name) + "_text";
			if (out_config_file) {
				if (std::filesystem::exists(path)) {
					out_config_file = false;
					path /= repacker::config::name;
					goto _out;
				}
			}
			if (out_path_create(path, ".txt")) goto _out;
		}
		throw std::exception("Failed to create file or The mes file version is not supported!");
	_out:
		wr_buf.save(path.string().c_str());

	}
}