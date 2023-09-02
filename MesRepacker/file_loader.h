#pragma once

namespace file {

	FileHelper  ::TextReader file_tr;
	StringHelper::UTF8String text;
	uint8_t read_state = 0;

	bool load_file(std::string path) {
		return file::file_tr.reader(path.c_str()).has_data();
	}

	template <typename text_reader>
	bool read_file(std::string path, text_reader reader) {
		file::read_state = 0;
		bool is_readed   = file::load_file(path);
		if ( is_readed ) { file_tr.foreach(reader); }
		return is_readed;
	}

	void read_config(int, char* str) {
		using namespace configuration;
		file::text.assign(str).trim();
		if (!text.get_length()||!text.get_c_strlen()) {
			file::read_state = 0;
		}
		else if (text.equals(repacker::config::path)) {
			file::read_state = 1;
		}
		else if (text.equals(repacker::config::cdpg)) {
			file::read_state = 2;
		}
		else if (text.equals(repacker::config::tmin)) {
			file::read_state = 3;
		}
		else if (text.equals(repacker::config::tmax)) {
			file::read_state = 4;
		}
		else if (text.equals(repacker::config::bfrp)) {
			file::read_state = 5;
		}
		else if (text.equals(repacker::config::atrp)) {
			file::read_state = 6;
		}
		else if (repacker::config::r1(file::read_state)) {
			repacker::input_path.assign(str);
		}
		else if (repacker::config::r2(file::read_state)) {
			repacker::set_code_pege(text.get_c_str());
		}
		else if (repacker::config::r3(file::read_state)) {
			repacker::set_min_length(text.get_c_str());
		}
		else if (repacker::config::r4(file::read_state)) {
			repacker::set_max_length(text.get_c_str());
		}
		else if (repacker::config::r5(file::read_state)) {
			repacker::add_replaces(text, true);
		}
		else if (repacker::config::r6(file::read_state)) {
			repacker::add_replaces(text, false);
		}
	}

	bool read_config_if_exists(std::string path) {
		using namespace configuration::repacker::config;
		return file::read_file(path + "\\" + name, read_config);
	}

	void run_replaces(std::string& o, std::string& n) {
		file::text.replace(o.c_str(), n.c_str());
	}

	void read_text_formats_to_maps() {
		using namespace configuration;
		mes_helper::text_map::init();
		int32_t position = 0, split = -1;
		file_tr.foreach([&](int, char* str) -> void {
			file::text.assign(str).trim();
			if (!text.get_length() || !text.get_c_strlen()) {
				file::read_state = 0;
			}
			else if (text.start_with(u8"#0x")) {
				file::text.remove(u8"#");
				if (init::is_not_convert) {
					file::text.replace("\\n", "\n");
					file::read_state = 3;
				}
				else if(sscanf(text.get_c_str(), "%x", &position)) {
					file::read_state = 1;
				}
			}
			else if (file::read_state == 1) {
				if (file::text.find(u8"◎★//") != -1) {
					file::read_state = 2;
				}
			}
			else if (file::read_state == 2) {
				using namespace mes_helper;
				split = file::text.find(u8"◎★");
				if (split != -1 && position) {
					text.assign((utf8str &) text.substr(split + 2));
					repacker::replaces_foreach(run_replaces,  true);
					text::formater(text.get_c_str(),  &text, false);
					repacker::replaces_foreach(run_replaces, false);
					text_map::add(position, text.get_c_str());
				}
				position = 0, split = -1;
				file::read_state    =  0;
			}
			if (file::read_state == 3) {
				using namespace mes_helper;
				std::string n_text(text.get_c_str()), offset;
				if ((split = n_text.find_first_of(":")) != std::string::npos) {
					offset.assign(n_text.substr(0, split));
					if (sscanf(offset.c_str(), "%x", &position)) {
						n_text.assign(n_text.substr(split + 1));
						text_map::add(position, n_text.c_str());
					}
				}
				position = 0, split = -1;
				file::read_state = 0;
			}
		});
		file::read_state = 0;
	}
}