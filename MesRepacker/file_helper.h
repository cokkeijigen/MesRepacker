#pragma once

namespace files {

    class readbuffer {
    protected:
        FILE* file_ptr;
        uint8_t* buffer;
        uint8_t* tmpbuf;
        size_t   bfsize;
        bool   is_clear;

    public:
        readbuffer(const char* filepath) : readbuffer() { read(filepath); }
        readbuffer() : buffer(nullptr), file_ptr(nullptr), tmpbuf(nullptr), bfsize(0), is_clear(true) { }

        size_t load(const char* filepath) {
            if (file_ptr = fopen(filepath, "rb")) {
                fseek(file_ptr, 0, SEEK_END);
                bfsize = ftell(file_ptr);
            }
            return bfsize;
        }

        uint8_t* read(size_t pos, size_t size) {
            if (tmpbuf != nullptr) delete[] buffer;
            if (size > 0) tmpbuf = new uint8_t[size];
            if (pos >= bfsize) return NULL;
            if (file_ptr != nullptr && size > 0) {
                fseek(file_ptr, 0, pos);
                fread(tmpbuf, 1, size, file_ptr);
            }
            else if (buffer && size > 0) {
                size_t copy = bfsize - pos;
                copy = size > copy ? (size - copy) : size;
                memcpy(tmpbuf, buffer, size);
            }
            return tmpbuf;
        }

        readbuffer& read(const char* filepath = nullptr) {
            if (filepath == nullptr && file_ptr && is_clear) {
                if (buffer) delete[] buffer;
                buffer = new uint8_t[bfsize];
                fseek(file_ptr, 0, SEEK_SET);
                fread(buffer, 1, bfsize, file_ptr);
                fclose(this->file_ptr);
                this->file_ptr = nullptr;
                this->is_clear = false;
            }
            else if (filepath != nullptr) {
                if (!this->is_clear) this->clear();
                FILE* file = fopen(filepath, "rb");
                if (file != NULL) {
                    fseek(file, 0, SEEK_END);
                    bfsize = ftell(file);
                    buffer = new uint8_t[bfsize];
                    fseek(file, 0, SEEK_SET);
                    fread(buffer, 1, bfsize, file);
                    this->is_clear = false;
                    fclose(file);
                }
            }
            return *this;
        }

        size_t size() {
            return this->bfsize;
        }

        bool empty() {
            return !this->bfsize;
        }

        uint8_t* data() {
            return this->buffer;
        }

        readbuffer& clear() {
            if (buffer) delete[] buffer;
            if (tmpbuf) delete[] tmpbuf;
            if (file_ptr) fclose(file_ptr);
            this->is_clear = true;
            this->file_ptr = nullptr;
            this->buffer = nullptr;
            this->tmpbuf = nullptr;
            this->bfsize = 0x00;
            return *this;
        }

        ~readbuffer() { clear(); }
    };

    class writebuffer {
    protected:
        uint8_t* buffer;
        size_t bfsize;
        size_t length;
        size_t exsize;
        size_t initsz;
    private:
        void initialize(bool init) {
            if (buffer) delete[] buffer;
            bfsize = initsz;
            length = 0x00;
            if (!init) return;
            buffer = new uint8_t[initsz];
        }

        void expandbuffer(size_t bfsz) {
            if (bfsz == NULL) {
                bfsize = bfsize + exsize;
            }
            else {
                size_t bfnsz = length + bfsz;
                if (bfnsz < bfsize) return;
                while (bfnsz >= (bfsize += exsize));
            }
            uint8_t* nbuf = new uint8_t[bfsize];
            if (length && buffer) {
                memcpy(nbuf, buffer, length);
                delete[] buffer;
            }
            buffer = nbuf;
        }

    public:
        writebuffer() : writebuffer(1024, 500) {}
        writebuffer(size_t initsz, size_t exsize) : initsz(initsz), exsize(exsize) {
            this->initialize(true);
        }
        ~writebuffer() { this->initialize(false); }

        writebuffer& write(const uint8_t* buf, size_t bfsz) {
            this->expandbuffer(bfsz);
            memcpy(&buffer[length], buf, bfsz);
            length = length + bfsz;
            return *this;
        }

        writebuffer& rewrite(size_t pos, const int8_t* bytes, size_t bfsz) {
            if (pos + bfsz >= length) expandbuffer(NULL);
            memcpy(&this->buffer[pos], bytes, bfsz);
            return *this;
        }

        template<typename... arguments>
        writebuffer& format_write(const char* fomat, size_t bfsize, arguments... args) {
            writebuffer::expandbuffer(bfsize);
            char* bufbeg = (char*)&buffer[length];
            sprintf(bufbeg, fomat, args...);
            length = length + strlen(bufbeg);
            return *this;
        }

        uint8_t* data() {
            return this->buffer;
        }

        size_t size() {
            return this->length;
        }

        bool save(const char* path) {
            FILE* outfp = fopen(path, "wb");
            if (outfp == NULL) return false;
            fwrite(buffer, 1, length, outfp);
            fclose(outfp);
            return true;
        }

        writebuffer& clear() {
            if (length) this->initialize(true);
            return *this;
        }
    };

    class textrbfr: public readbuffer {
        struct line { size_t offset; size_t length; };
        uint8_t* read(size_t, size_t) {}
        readbuffer& read(const char*) {}
        readbuffer& read() {}
    private:
        size_t lines;
        line*  arrle;
        size_t arrsz;
        size_t maxlen;
        char* outtext;
    protected:
        inline void addline(size_t offset, size_t length) {
            if (!length) return;
            if (!this->arrsz || this->lines >= this->arrsz) {
                size_t length = this->arrsz + 100;
                line* new_arr = new line[length];
                if (this->arrsz) {
                    memcpy(new_arr, arrle, sizeof(line) * arrsz);
                    delete[] this->arrle;
                }
                this->arrle = new_arr;
                this->arrsz = length;
            }
            maxlen = length > maxlen ? length : maxlen;
            this->arrle[this->lines++] = { offset, length };
        }

        inline void parsing(bool line_breaks = true) {
            uint8_t* buf_end = buffer + bfsize;
            uint8_t* buf_tmp = buffer;
            uint8_t* current = buf_tmp;
            uint8_t symbol[] = "\r\n";
            bool while_break = false;
            int8_t line_break = 0;
            do {
                if (buf_tmp != (buf_tmp - 1) && *buf_tmp == '\r'
                    && *(uint16_t*)buf_tmp == *(uint16_t*)symbol) {
                    line_break = 2, buf_tmp += 1;
                }
                else if (*buf_tmp == '\n' || *buf_end == '\r') {
                    line_break = 1;
                }
                while_break = ++buf_tmp == buf_end;
                if (line_break || while_break) {
                    uint32_t length = buf_tmp - current;
                    uint32_t offset = current - buffer;
                    length -= line_breaks ? 0 : line_break;
                    this->addline(offset, length);
                    line_break = 0;
                    current = buf_tmp;
                }
            } while (!while_break);
            if (maxlen) outtext = new char[maxlen + 1];
            textrbfr::is_clear = false;
        }

        void init() {
            if(this->arrle)   delete[] arrle;
            if(this->outtext) delete[] outtext;
            this->lines  = NULL;
            this->arrsz  = NULL;
            this->maxlen = NULL;
            this->arrle  = nullptr;
            this->outtext= nullptr;
            this->is_clear =  true;
        }
    public:
        textrbfr(const char* path): readbuffer::readbuffer(path) { textrbfr::init(); }
        textrbfr(): readbuffer::readbuffer(){ textrbfr::init(); }
        
        textrbfr& load(const char* path, bool line_breaks = true) {
            if (!is_clear) clear();
            readbuffer::read(path);
            if (!empty())parsing();
            return *this;
        }

        textrbfr& clear() {
            readbuffer::clear();
            textrbfr::init();
            return *this;
        }

        size_t count() {
            return this->lines;
        }

        const char* at(size_t num) {
            line* atline = &this->arrle[num];
            uint8_t* text = &this->buffer[atline->offset];
            memcpy(this->outtext, text, atline->length);
            this->outtext[atline->length] = '\0';
            return this->outtext;
        }

        template <typename funcallback>
        void foreach(funcallback callback) {
            for (size_t i = 0; i < this->lines; i++) {
                callback(i, (char*)this->at(i));
            }
        }
        ~textrbfr() {
            this->clear();
        }
    };
}