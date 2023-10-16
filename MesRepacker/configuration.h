#pragma once

namespace configuration {

	struct key_section {
		uint8_t beg, end;
		bool with(uint8_t key) {
			return (
				(!(beg == end && beg == 0xFF))
				&& (key >= beg && key <= end)
			);
		}
	};

	typedef struct {
		const char  name[10];
		uint16_t     version;
		key_section	 uint8x2;
		key_section uint8str;
		key_section	  string;
		key_section   decstr;
		key_section uint16x4;
		uint8_t     optundec;
	} mes_config;
}

namespace configuration::mes {
	
	std::vector<mes_config> configs;

	void init() {

		configs.push_back({
			"ffexa", 0x7B69,
			{0x00, 0x28},
			{0x29, 0x2E},
			{0x2F, 0x49},
			{0x4A, 0x4D},
			{0x4E, 0xFF},
			 0x43
		});

		configs.push_back({
			"ffexs", 0x7B6B,
			{0x00, 0x28},
			{0x29, 0x2E},
			{0x2F, 0x4B},
			{0x4c, 0x4F},
			{0x50, 0xFF},
			 0x43
		});

		configs.push_back({
			"ef", 0x466A,
			{0x00, 0x28},
			{0x2A, 0x2F},
			{0x30, 0x4A},
			{0x4B, 0x4E},
			{0x4F, 0xFF},
			 0x46
		});

		configs.push_back({
			"dcos",0x315D,
			{0x00, 0x2B},
			{0xFF, 0xFF}, // 0xFF占位
			{0x2C, 0x45},
			{0x46, 0x49},
			{0x4A, 0xFF},
			 0x42
		});

		configs.push_back({
			"ktlep",0x6E69,
			{0x00, 0x28},
			{0x29, 0x2E},
			{0x2F, 0x49},
			{0x4A, 0x4D},
			{0x4E, 0xFF},
			 0x45
		});

		configs.push_back({
			"dcws",0x656C,
			{0x00, 0x2B},
			{0x2C, 0x31},
			{0x32, 0x4C},
			{0x4D, 0x50},
			{0x51, 0xFF},
			 0x48
		});

		configs.push_back({
			"dcsv",0x636C,
			{0x00, 0x2B},
			{0x2C, 0x31},
			{0x32, 0x4C},
			{0x4D, 0x50},
			{0x51, 0xFF},
			 0x46
		});

		configs.push_back({
			"dcpc",0x3D63,
			{0x00, 0x2C},
			{0xFF, 0xFF},
			{0x2D, 0x49},
			{0x4A, 0x4D},
			{0x4E, 0xFF},
			 0x44
		});

		configs.push_back({
			"dcmems",0x315D,
			{0x00, 0x2B},
			{0xFF, 0xFF},
			{0x2C, 0x45},
			{0x46, 0x49},
			{0x4A, 0xFF},
			 0x42
		});

		configs.push_back({
			"dcdx",0x7769,
			{0x00, 0x28},
			{0x29, 0x2E},
			{0x2F, 0x49},
			{0x4A, 0x4D},
			{0x4E, 0xFF},
			 0x45
		});

		configs.push_back({
			"dcas",0x4E69,
			{0x00, 0x28},
			{0x29, 0x2E},
			{0x2F, 0x49},
			{0x4A, 0x4D},
			{0x4E, 0xFF},
			 0x43
		});

		configs.push_back({
			"dcbs",0x3163,
			{0x00, 0x2B},
			{0xFF, 0xFF}, // 0xFF占位
			{0x2C, 0x48},
			{0x49, 0x4C},
			{0x4D, 0xFF},
			 0x00 // 占位
		});

		configs.push_back({
			"dc2fl",0x9C69,
			{0x00, 0x28},
			{0x29, 0x2E},
			{0x2F, 0x49},
			{0x4A, 0x4D},
			{0x4E, 0xFF},
			 0x45
		});

		configs.push_back({
			"dc2bs",0x316C,
			{0x00, 0x2B},
			{0x2C, 0x31},
			{0x32, 0x4C},
			{0x4D, 0x50},
			{0x51, 0xFF},
			 0x00 // 占位
		});

		configs.push_back({
			"dc2dm",0x9D72,
			{0x00, 0x29},
			{0x2A, 0x31},
			{0x32, 0x4C},
			{0x4D, 0x50},
			{0x51, 0xFF},
			 0x44
		});

		configs.push_back({
			"dc2fy",0x3866,
			{0x00, 0x2E},
			{0xFF, 0xFF}, // 0xFF占位
			{0x2F, 0x4B},
			{0x4C, 0x4F},
			{0x50, 0xFF},
			 0x48
		});

		configs.push_back({
			"dc2cckko",0x026C,
			{0x00, 0x2B},
			{0x2C, 0x31},
			{0x32, 0x4C},
			{0x4D, 0x50},
			{0x51, 0xFF},
			 0x00 // 占位
		});

		configs.push_back({
			"dc2ccotm",0x016C,
			{0x00, 0x2B},
			{0x2C, 0x31},
			{0x32, 0x4C},
			{0x4D, 0x50},
			{0x51, 0xFF},
			 0x00 // 占位
		});

		configs.push_back({
			"dc2sc",0x3B69,
			{0x00, 0x28},
			{0x29, 0x2E},
			{0x2F, 0x49},
			{0x4A, 0x4D},
			{0x4E, 0xFF},
			 0x45
		});

		configs.push_back({
			"dc2ty",0x5F69,
			{0x00, 0x28},
			{0x29, 0x2E},
			{0x2F, 0x49},
			{0x4A, 0x4D},
			{0x4E, 0xFF},
			 0x00
		});

		configs.push_back({
			"dc2pc",0x5769,
			{0x00, 0x28},
			{0x29, 0x2E},
			{0x2F, 0x49},
			{0x4A, 0x4D},
			{0x4E, 0xFF},
			 0x45
		});

		configs.push_back({
			"dc3rx",0x9772,
			{0x00, 0x2B},
			{0x2C, 0x33},
			{0x34, 0x4E},
			{0x4F, 0x52},
			{0x53, 0xFF},
			 0x45
		});

		configs.push_back({
			"dc3pp",0x9872,
			{0x00, 0x2A},
			{0x2B, 0x32},
			{0x33, 0x4E},
			{0x4F, 0x51},
			{0x52, 0xFF},
			 0x45
		});

		configs.push_back({
			"dc3wy",0xA09F,
			{0x00, 0x38},
			{0x39, 0x41},
			{0x42, 0x5F},
			{0x60, 0x63},
			{0x64, 0xFF},
			 0x55
		});

		configs.push_back({
			"dc3dd",0xA5A8,
			{0x00, 0x38},
			{0x39, 0x43},
			{0x44, 0x62},
			{0x63, 0x67},
			{0x68, 0xFF},
			 0x58
		});

		configs.push_back({
			"dc4",0xAAB6,
			{0x00, 0x3A},
			{0x3B, 0x47},
			{0x48, 0x68},
			{0x69, 0x6D},
			{0x6E, 0xFF},
			 0x5D
		});

		configs.push_back({
			"dc4ph",0xABB6,
			{0x00, 0x3A},
			{0x3B, 0x47},
			{0x48, 0x68},
			{0x69, 0x6D},
			{0x6E, 0xFF},
			 0x5D
		});

		configs.push_back({
			"ds",0x9F9A,
			{0x00, 0x38},
			{0x39, 0x4A},
			{0x41, 0x5E},
			{0x5F, 0x62},
			{0x63, 0xFF},
			 0x54
		});

		configs.push_back({
			"dsif",0xA1A1,
			{0x00, 0x39},
			{0x3A, 0x42},
			{0x43, 0x60},
			{0x61, 0x64},
			{0x65, 0xFF},
			 0x56
		});
	}

	void clear() {
		configs.clear();
	}
}

namespace configuration::init {

	mes_config* config  = nullptr;
	bool is_not_convert = false;
	bool is_input_gbk   = false;

	void find_not_convert(std::string exe_name) {
		is_not_convert = exe_name.find("-nsc") != std::string::npos;
	}

	void find_iput_gbk(std::string exe_name) {
		is_input_gbk = exe_name.find("-igbk") != std::string::npos;
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
}

namespace configuration::repacker {
	std::string input_path;
	uint32_t use_code_page = 936;
	int32_t text_max_length = 0;
	int32_t text_min_length = 0;
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
		const char* name = u8".MesRepacker";
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

	bool mes_is_exists(std::string file) {
		current_file.assign(input_path) /= file.append(".mes");
		return std::filesystem::exists(current_file);
	}

	void set_code_pege(const char* cp_str) {
		try {
			use_code_page = std::stoi(cp_str);
		}
		catch (const std::exception&) {}
	}

	void set_max_length(const char* max) {
		try {
			text_max_length = std::stoi(max);
		}
		catch (const std::exception&) {}
	}

	void set_min_length(const char* min) {
		try {
			text_min_length = std::stoi(min);
		}
		catch (const std::exception&) {}
	}

	void add_replaces(StringHelper::UTF8String& text, bool is_before) {
		StringHelper::UTF8String strtmp1, strtmp2;
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
		for (auto& pair : replaces) handler(
			(std::string&)pair.first, (std::string&)pair.second
		);
	}
}
