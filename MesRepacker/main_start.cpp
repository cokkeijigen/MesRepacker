#define _CRT_SECURE_NO_WARNINGS
#define _Version 1.1
#include "includes.h"

namespace worker {

	std::string exe_name ;
	std::string work_path;
	std::string file_path;
	
	void try_get_config_in_exe_name() {
		using namespace configuration;
		init::find_iput_gbk(exe_name);
		init::find_not_convert(exe_name);
		mes ::find_config(exe_name);
	}

	void init(const char* path1, const char* path2) {
		worker::work_path.assign(path1);
		worker::file_path.assign(path2);
		size_t syslast = work_path.find_last_of("\\") + 1;
		exe_name.assign (work_path.substr(   syslast));
		work_path.assign(work_path.substr(0, syslast));
		for (char& c : exe_name) c = std::tolower(c);
		worker::try_get_config_in_exe_name();
	}

	void run_repacker() {
		if (configuration::repacker::exist_path()) {
			using namespace configuration;
			using namespace std::filesystem;
			for (auto& f : directory_iterator(file_path)) {
				std::string exts = f.path().extension().string();
				std::string name = f.path().stem().string();
				if (exts.empty() || exts != ".txt") continue;
				if (!repacker::mes_is_exists(name)) continue;
				try {
					if (file::load_file(f.path().string())) {
						mes_helper::loader::file_repacker();
						mes_helper::loader::mes_loading();
						file::read_text_formater_to_map();
						mes_helper::loader::write_clear();
						mes_helper::loader::import_text();
						mes_helper::loader::out(work_path);
						printf("mes saved: %s.mes\n", name.c_str());
					}
					else throw std::exception("read failure!");
				}
				catch (std::exception& e) {
					printf("%s: %s\n", name.c_str(), e.what());
				}
			}
		}
		else {
			throw std::exception("The mes file path does not exist!");
		}
	}

	void exporter_as_afile(std::filesystem::path path) {
		std::string ext = path.extension().string();
		std::string name = path.stem().string();
		if (ext.empty() || ext != ".mes") return;
		try {
			mes_helper::loader::cur_file = path;
			mes_helper::loader::file_exporter();
			mes_helper::loader::mes_loading();
			mes_helper::loader::write_clear();
			mes_helper::loader::export_text();
			mes_helper::loader::out(work_path);
			printf("text saved: %s.txt\n", name.c_str());
		}
		catch (std::exception& e) {
			printf("%s.mes: %s\n", name.c_str(), e.what());
		}
	}

	void exporter_as_multifile() {
		using namespace std::filesystem;
		for (auto& file : directory_iterator(file_path)) {
			exporter_as_afile(file.path());
		}
		mes_helper::loader::cur_file = file_path;
		mes_helper::loader::create_config();
		mes_helper::loader::out(work_path);
	}

	void opt_log(clock_t satrt, clock_t end) {
		double run_time = (double)(end - satrt) / 1000;
		std::cout << "\n--------------------------" << std::endl;
		std::cout << "time consuming: " << run_time << std::endl;
		std::cout << "MesRepacker ver " << _Version << std::endl;
		std::cout << "https://github.com/cokkeijigen/MesRepacker";
		std::cout << std::endl;
		std::cin.ignore();
	}

	void on_handler_start() {
		if (file::read_config_if_exists(file_path)) {
			mes_helper::text::config_init();
			worker::run_repacker();
		}
		else if(std::filesystem::is_directory(file_path)){
			worker::exporter_as_multifile();
		}
		else if (std::filesystem::exists(file_path)) {
			worker::exporter_as_afile(file_path);
			mes_helper::loader::create_config();
			mes_helper::loader::out(work_path);
		}
		else throw std::exception("Unknown error!");
	}
}

int main(int argc, char* argv[]) {
	clock_t start_t = clock();
	try {
		if (argc == 2) { 
			worker::init(argv[0], argv[1]);
			configuration::mes::init();
			worker::on_handler_start();
		}
		else throw std::exception("Illegal parameter!!!");
	}
	catch (std::exception& e) {
		printf("%s\n", e.what());
	}
	configuration::mes::clear();
	worker::opt_log(start_t, clock());
	return 0;
}