#pragma once

std::string sj2utf8(char* str) {
    DWORD dwNum = MultiByteToWideChar(932, 0, str, -1, NULL, 0);
    wchar_t* dst = new wchar_t[dwNum];
    MultiByteToWideChar(932, 0, str, -1, dst, dwNum);
    dwNum = WideCharToMultiByte(CP_UTF8, NULL, dst, -1, NULL, 0, NULL, FALSE);
    char* psText = new char[dwNum];
    WideCharToMultiByte(CP_UTF8, NULL, dst, -1, psText, dwNum, NULL, FALSE);
    std::string res = std::string(psText);
    delete[] psText;
    delete[] dst;
    return res;
}

std::string gbk2utf8(char* str) {
    int dwNum = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
    wchar_t* dst = new wchar_t[dwNum];
    MultiByteToWideChar(CP_ACP, 0, str, -1, dst, dwNum);
    dwNum = WideCharToMultiByte(CP_UTF8, 0, dst, -1, NULL, 0, NULL, FALSE);
    char* psText = new char[dwNum];
    WideCharToMultiByte(CP_UTF8, 0, dst, -1, psText, dwNum, NULL, FALSE);
    std::string res = std::string(psText);
    delete[] psText;
    delete[] dst;
    return res;
}

std::string strcon(std::string str, int ncp) {
    if (!ncp || ncp == CP_UTF8) return str;
    int dwNum = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t* dst = new wchar_t[dwNum + 1];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, dst, dwNum);
    dwNum = WideCharToMultiByte(ncp, 0, dst, -1, NULL, 0, NULL, NULL);
    char* psText = new char[dwNum + 1];
    WideCharToMultiByte(ncp, 0, dst, -1, psText, dwNum, NULL, NULL);
    std::string res(psText);
    delete[]dst;
    delete[]psText;
    return res;

}

void replacestr(std::string &orgstr, std::string oldstr, std::string newstr, size_t start = 0) {
    while ((start = orgstr.find(oldstr, start)) != std::string::npos) {
        orgstr.replace(start, oldstr.length(), newstr);
        start += newstr.length();
    }
}