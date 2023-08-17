#pragma once

namespace configuration {

	struct key_section {
		uint8_t start = 0;
		uint8_t end = 0;
		bool with(uint8_t key) {
			return ((!(start == end && start == 0xFF)) && (key >= start && key <= end));
		}
	};

	struct mes_config {
		int32_t(*offset)(int32_t);
		uint16_t	head_t;
		char		name[10];
		key_section uint8x2;
		key_section uint8str;
		key_section str;
		key_section decstr;
		key_section shortx4;
		uint8_t		opt_undec;
	};

	int32_t offset_bfr(int32_t start){
		return start * 4 + 4;
	}

	int32_t offset_aft(int32_t start) {
		return start * 6 + 4;
	}
}

namespace configuration::init {
	mes_config* config  = NULL;
	bool is_not_convert = false;
	bool is_input_gbk   = false;

	void find_not_convert(std::string exe_name) {
		is_not_convert = exe_name.find("-nsc") != std::string::npos;
	}

	void find_iput_gbk(std::string exe_name) {
		is_input_gbk = exe_name.find("-igbk") != std::string::npos;
	}
}

namespace configuration::repacker {
	
	std::string input_path;
	uint32_t use_code_page   = 936;
	uint32_t text_max_length = 0;
	uint32_t text_min_length = 0;
	std::filesystem::path current_file;
	std::map<std::string, std::string> befors_replaces;
	std::map<std::string, std::string> after_replaces;

	namespace config {
		typedef bool(*f)(uint8_t);
		const char* path = u8"#InputPath";
		const char* cdpg = u8"#UseCodePage";
		const char* tmin = u8"#Text-MinLength";
		const char* tmax = u8"#Text-MaxLength";
		const char* bfrp = u8"#Before-Replaces";
		const char* atrp = u8"#After-Replaces";
		const char* name = u8"\\.MesRepacker";
		f r1 = [](uint8_t s) -> bool { return s == 1; };
		f r2 = [](uint8_t s) -> bool { return s == 2; };
		f r3 = [](uint8_t s) -> bool { return s == 3; };
		f r4 = [](uint8_t s) -> bool { return s == 4; };
		f r5 = [](uint8_t s) -> bool { return s == 5; };
		f r6 = [](uint8_t s) -> bool { return s == 6; };
	}

	bool exist_path() {
		return input_path.length() && std::filesystem::exists(input_path);
	}

	bool is_exists(std::string file) {
		current_file.assign(input_path) /= file.append(".mes");
		return std::filesystem::exists(current_file);
	}

	void set_code_pege(const char * cp_str) {
		try {
			use_code_page = std::stoi(cp_str);
		} catch (const std::exception&) {}
	}

	void set_max_length(const char* max) {
		try {
			text_max_length = std::stoi(max);
		}
		catch (const std::exception&) {}
	}

	void set_min_length(const char* max) {
		try {
			text_min_length = std::stoi(max);
		}
		catch (const std::exception&) {}
	}

	void add_replaces(StringHelper::UTF8String & text, bool is_before) {
		StringHelper::UTF8String strtmp1 , strtmp2;
		std::pair<std::string, std::string> n_pair;
		int32_t split = text.find_last_of(u8"]:[");
		if (split != -1 && split != 1 && (text.get_length() - split + 3)
			&& text.start_with(u8"[") && text.end_with(u8"]")) {
			strtmp1 = text.substr(1, split);
			strtmp2 = text.substr(split + 3, -1);
			if (strtmp1.get_length() && strtmp1.get_length()) {
				const char* old_str = strtmp1.get_c_str();
				const char* new_str = strtmp2.get_c_str();
				n_pair = std::make_pair(old_str, new_str);
				if (is_before) {
					befors_replaces.insert(n_pair);
				}
				else {
					after_replaces.insert(n_pair);
				}
			}
			strtmp1.clear(), strtmp2.clear();
		}
	}

	template <typename funcallback>
	void replaces_foreach(funcallback handler, bool is_befor) {
		auto replaces = is_befor ? befors_replaces : after_replaces;
		if (replaces.empty() && replaces.size() < 1) return;
		for (auto& pair : replaces) handler (
			(std::string&)pair.first, (std::string&)pair.second
		);
	}
}

namespace configuration::mes {
	
	std::vector<mes_config> configs;

	void init() {

		configs.push_back({
			offset_bfr,
			0x6A46, "ef",
			{ 0x00, 0x29 },
			{ 0x2A, 0x2F },
			{ 0x30, 0x4b },
			{ 0x4b, 0x4E },
			{ 0x4F, 0xFF },
			  0x46
		});

		configs.push_back({
			offset_bfr,
			0x5D31, "dcos",
			{ 0x00, 0x2B },
			{ 0xFF, 0xFF }, // 0xFF鍗犱綅
			{ 0x2C, 0x45 },
			{ 0x46, 0x49 },
			{ 0x4A, 0xFF },
			  0x42
		});

		configs.push_back({
			offset_bfr,
			0x696E, "ktlep",
			{ 0x00, 0x28 },
			{ 0x29, 0x2E },
			{ 0x2F, 0x49 },
			{ 0x4A, 0x4D },
			{ 0x4E, 0xFF },
			  0x45
		});

		configs.push_back({
			offset_bfr,
			0x6C65, "dcws",
			{ 0x00, 0x2B },
			{ 0x2C, 0x31 },
			{ 0x32, 0x4C },
			{ 0x4D, 0x50 },
			{ 0x51, 0xFF },
			  0x48
		});

		configs.push_back({
			offset_bfr,
			0x6C63, "dcsv",
			{ 0x00, 0x2B },
			{ 0x2C, 0x31 },
			{ 0x32, 0x4C },
			{ 0x4D, 0x50 },
			{ 0x51, 0xFF },
			  0x46
		});

		configs.push_back({
			offset_bfr,
			0x633D, "dcpc",
			{ 0x00, 0x2C },
			{ 0xFF, 0xFF },
			{ 0x2D, 0x49 },
			{ 0x4A, 0x4D },
			{ 0x4E, 0xFF },
			  0x44
		});

		configs.push_back({
			offset_bfr,
			0x5D31, "dcmems",
			{ 0x00, 0x2B },
			{ 0xFF, 0xFF },
			{ 0x2C, 0x45 },
			{ 0x46, 0x49 },
			{ 0x4A, 0xFF },
			  0x42
		});

		configs.push_back({
			offset_bfr,
			0x6977, "dcdx",
			{ 0x00, 0x28 },
			{ 0x29, 0x2E },
			{ 0x2F, 0x49 },
			{ 0x4A, 0x4D },
			{ 0x4E, 0xFF },
			  0x45
		});

		configs.push_back({
			offset_bfr,
			0x694E, "dcas",
			{ 0x00, 0x28 },
			{ 0x29, 0x2E },
			{ 0x2F, 0x49 },
			{ 0x4A, 0x4D },
			{ 0x4E, 0xFF },
			  0x43
		});

		configs.push_back({
			offset_bfr,
			0x6331, "dcbs",
			{ 0x00, 0x2B },
			{ 0xFF, 0xFF }, // 0xFF鍗犱綅
			{ 0x2C, 0x48 },
			{ 0x49, 0x4C },
			{ 0x4D, 0xFF },
			  0x00 // 鍗犱綅
		});

		configs.push_back({
			offset_bfr,
			0x699C, "dc2fl",
			{ 0x00, 0x28 },
			{ 0x29, 0x2E },
			{ 0x2F, 0x49 },
			{ 0x4A, 0x4D },
			{ 0x4E, 0xFF },
			  0x45
			});

		configs.push_back({
			offset_bfr,
			0x6C31, "dc2bs",
			{ 0x00, 0x2B },
			{ 0x2C, 0x31 },
			{ 0x32, 0x4C },
			{ 0x4D, 0x50 },
			{ 0x51, 0xFF },
			  0x00 // 鍗犱綅
			});

		configs.push_back({
			offset_aft,
			0x729D, "dc2dm",
			{ 0x00, 0x29 },
			{ 0x2A, 0x31 },
			{ 0x32, 0x4C },
			{ 0x4D, 0x50 },
			{ 0x51, 0xFF },
			  0x44 
			});

		configs.push_back({
			offset_bfr,
			0x6638, "dc2fy",
			{ 0x00, 0x2E },
			{ 0xFF, 0xFF }, // 0xFF鍗犱綅
			{ 0x2F, 0x4B },
			{ 0x4C, 0x4F },
			{ 0x50, 0xFF },
			  0x48
			});

		configs.push_back({
			offset_bfr,
			0x6C02, "dc2cckko",
			{ 0x00, 0x2B },
			{ 0x2C, 0x31 },
			{ 0x32, 0x4C },
			{ 0x4D, 0x50 },
			{ 0x51, 0xFF },
			  0x00 // 鍗犱綅
			});

		configs.push_back({
			  offset_bfr,
			  0x6C01, "dc2ccotm",
			{ 0x00, 0x2B },
			{ 0x2C, 0x31 },
			{ 0x32, 0x4C },
			{ 0x4D, 0x50 },
			{ 0x51, 0xFF },
			  0x00 // 鍗犱綅
			});

		configs.push_back({
			offset_bfr,
			0x693B, "dc2sc",
			{0x00, 0x28},
			{0x29, 0x2E},
			{0x2F, 0x49},
			{0x4A, 0x4D},
			{0x4E, 0xFF},
			0x45
			});

		configs.push_back({
			  offset_bfr,
			  0x695F, "dc2ty",
			{ 0x00, 0x28 },
			{ 0x29, 0x2E },
			{ 0x2F, 0x49 },
			{ 0x4A, 0x4D },
			{ 0x4E, 0xFF },
			  0x00
		});

		configs.push_back({
			  offset_bfr,
			  0x6957, "dc2pc",
			{ 0x00, 0x28 },
			{ 0x29, 0x2E },
			{ 0x2F, 0x49 },
			{ 0x4A, 0x4D },
			{ 0x4E, 0xFF },
			  0x45
		});

		configs.push_back({
			offset_aft,
			0x7297, "dc3rx",
			{ 0x00, 0x2B },
			{ 0x2C, 0x33 },
			{ 0x34, 0x4E },
			{ 0x4F, 0x52 },
			{ 0x53, 0xFF },
			  0x45
		});

		configs.push_back({
			offset_aft,
			0x7298, "dc3pp",
			{ 0x00, 0x2A },
			{ 0x2B, 0x32 },
			{ 0x33, 0x4E },
			{ 0x4F, 0x51 },
			{ 0x52, 0xFF },
			  0x45
		});

		configs.push_back({
			offset_aft,
			0x9FA0, "dc3wy",
			{ 0x00, 0x38 },
			{ 0x39, 0x41 },
			{ 0x42, 0x5F },
			{ 0x60, 0x63 },
			{ 0x64, 0xFF },
			  0x55
		});

		configs.push_back({
			offset_aft,
			0xA8A5, "dc3dd",
			{ 0x00, 0x38 },
			{ 0x39, 0x43 },
			{ 0x44, 0x62 },
			{ 0x63, 0x67 },
			{ 0x68, 0xFF },
			  0x58
		});

		configs.push_back({
			offset_aft,
			0xB6AA, "dc4",
			{ 0x00, 0x3A },
			{ 0x3B, 0x47 },
			{ 0x48, 0x68 },
			{ 0x69, 0x6D },
			{ 0x6E, 0xFF },
			  0x5d
		});

		configs.push_back({
			offset_aft,
			0x9A9F, "ds",
			{ 0x00, 0x38 },
			{ 0x39, 0x4A },
			{ 0x41, 0x5E },
			{ 0x5F, 0x62 },
			{ 0x63, 0xFF },
			  0x54
		});
	}

	void find_config(std::string exe_name) {
		if (exe_name.find("-s") == std::string::npos) return;
		for (mes_config& config : mes::configs) {
			std::string find_config = std::string("-s") + config.name;
			if (exe_name.find(find_config) != std::string::npos) {
				configuration::init::config = &config;
				return;
			}
		}
	}

	void clear() {
		configs.clear();
	}
}