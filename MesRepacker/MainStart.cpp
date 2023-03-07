#include "Includes.h"

std::string exeName("");
std::string workPath("");
MesRepacker* mesRepacker = nullptr;
MesTextHelper* mesTextHelper = nullptr;

std::string CreatePath(std::string mespath, std::string path) {
	if (!std::filesystem::exists(path)) {
		system((std::string("MD \"") + path + "\"").c_str());
		if (!mespath.size()) goto _result;
		FILE* opt = fopen((path + ".MesRepacker").c_str(), "w");
		mespath.assign(mespath.substr(0, mespath.find_last_of("\\") + 1));
		fputs(mespath.c_str(), opt);
		fclose(opt);
	}
	_result:
	return path;
}

void StartHandleScript(char* filepath) {
	if (!mesTextHelper) mesTextHelper = new MesTextHelper(mesConfigs, exeName.find("-igbk") != -1);
	if (mesTextHelper->load(filepath)) {
		mesTextHelper->outTextToFile(CreatePath(filepath, (workPath + mesTextHelper->type_name + "_text\\")));
		std::cout << mesTextHelper->filename << ": done." << std::endl;
	}
	else
		std::cout << mesTextHelper->filename << ": failed!" << std::endl;
}

void StartHandleRepack(std::string textfile) {
	if (mesRepacker->load(textfile)) {
		mesRepacker->outMesFile(CreatePath("", (workPath + mesRepacker->type_name + "_mes\\")));
		std::cout << mesRepacker->filename << ": done." << std::endl;
	}else
		std::cout << mesRepacker->filename << ": failed!" << std::endl;
}

bool IsRepack(std::string path) {
	std::string mespath(path + "\\.MesRepacker");
	if (std::filesystem::exists(mespath) && !std::filesystem::is_directory(mespath)) {
		std::ifstream conf(mespath, std::ios::in);
		while (std::getline(conf, mespath)) if (mespath.size() > 1) break;
		if (!mesRepacker) mesRepacker = new MesRepacker(mesConfigs, mespath);
		for (auto& i : std::filesystem::directory_iterator(path)) {
			if (i.path().string().find(".txt") == -1) continue;
			StartHandleRepack(i.path().string());
		}
		conf.close();
		return true;
	}
	return false;
}

void OnHandleFiles(char* files) {
	if (std::filesystem::is_directory(files)) {
		if (IsRepack(files)) return;
		for (auto& i : std::filesystem::directory_iterator(files))
			StartHandleScript((char*)i.path().string().c_str());
	}
	else if (std::filesystem::exists(files)) {
		StartHandleScript(files);
	}
	else {
		std::cout << "err: " << files << std::endl;
	}
}

void test() {
	return;
	//OnHandleFiles((char*)"D:\\Galgame\\DC2DM\\Advdata\\MES\\mar_0124_c1.mes");
	//OnHandleFiles((char*)"D:\\Galgame\\DC3DD\\AdvData\\MES\\dd_7_0220_e1.mes");
}

int main(int argc, char* argv[]) {
	{
		exeName.assign(argv[0]);
		workPath.assign(argv[0]);
		exeName.assign(exeName.substr(exeName.find_last_of("\\") + 1));
		workPath.assign(workPath.substr(0, workPath.find_last_of("\\") + 1));
	}{
		initConf();
		system("@echo off");
		system("chcp 65001");
		if (argc != 2) test();
		else OnHandleFiles(argv[1]);
		if (mesTextHelper) mesTextHelper->destroy();
		if (mesRepacker) mesRepacker->destroy();
	}
	system("pause");
	return 0;
}