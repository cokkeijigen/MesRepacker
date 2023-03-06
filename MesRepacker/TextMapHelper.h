#pragma once

struct TextMap {
    int position;
    std::string text;
};

class TextMapHelper {
    std::vector<TextMap> textMaps;
public:

    TextMapHelper() {
    }

    ~TextMapHelper() {
        this->clear();
    }
    
    bool push(int pos, std::string str) {
        for (auto iter = this->textMaps.begin(); iter != this->textMaps.end(); iter++)
            if ((*iter).position == pos) return false;
        this->textMaps.push_back({pos, str});
        return true;
    }

    bool get(int pos, std::string &strbuf) {
        for (auto iter = this->textMaps.begin(); iter != this->textMaps.end(); iter++) {
            if ((*iter).position == pos) {
                strbuf.assign((*iter).text);
                return true;
            }
        }
        return false;
    }

    bool size() {
        return this->textMaps.size();
    }

    void clear() {
        this->textMaps.clear();
    }
    void foreach(void(*fun)(TextMap)) {
        for (auto iter = this->textMaps.begin(); iter != this->textMaps.end(); iter++) {
           (*fun)((*iter));
        }
    }

};