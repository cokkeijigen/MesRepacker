#pragma once

struct TextMap {
    int position;
    std::string text;
};

class TextMapHelper {
    std::map<int, std::string>* textMaps;
public:

    TextMapHelper() {
        this->textMaps = new std::map<int, std::string>;
    }

    ~TextMapHelper() {
        this->clear();
        if (this->textMaps) delete textMaps;
    }

    void push(int pos, std::string str) {
        this->textMaps->insert(std::make_pair(pos, str));
    }

    bool get(int pos, std::string& strbuf) {
        auto mapos = this->textMaps->find(pos);
        if (mapos != this->textMaps->end()) {
            strbuf.assign(mapos->second);
            return true;
        }
        else return false;
    }

    bool size() {
        return this->textMaps->size();
    }

    void clear() {
        this->textMaps->clear();
    }
    void foreach(void(*fun)(TextMap)) {
        for (auto iter = this->textMaps->begin(); iter != this->textMaps->end(); iter++) {
            (*fun)({(*iter).first, (*iter).second});
        }
    }

};