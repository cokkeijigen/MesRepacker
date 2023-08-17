#pragma once

namespace FileHelper {

	class ReadBuffer {

		uint8_t* buffer;
		size_t  buf_size;

	protected:
		void initialization() {
			if (buffer) delete[] buffer;
			this->buffer = nullptr;
			this->buf_size = 0;
		}

		void read_file(FILE* file) {
			fseek(file, 0, SEEK_END);
			buf_size = ftell(file);
			if (!buf_size) return;
			buffer = new uint8_t[buf_size];
			fseek(file, 0, SEEK_SET);
			fread(buffer, 1, buf_size, file);
			fclose(file);
		}
	public:
		ReadBuffer() { initialization(); }
		~ReadBuffer() { initialization(); }

		ReadBuffer& reader(FILE* file) {
			initialization();
			read_file(file);
			return *this;
		}

		ReadBuffer& reader(const char* filepath) {
			return reader(fopen(filepath, "rb"));
		}

		bool has_data() {
			return buf_size && buffer;
		}

		size_t size() {
			return buf_size;
		}

		const uint8_t* data() {
			return buffer;
		}

		int8_t get_int8(size_t pos) {
			return pos < buf_size - 1 ? *(buffer + pos) : 0;
		}

		int16_t get_int16(size_t pos) {
			return pos < buf_size - 2 ? *(int16_t*)(buffer + pos) : 0;
		}

		int32_t get_int32(size_t pos) {
			return pos < buf_size - 4 ? *(int32_t*)(buffer + pos) : 0;
		}

		int64_t get_int64(size_t pos) {
			return pos < buf_size - 8 ? *(int32_t*)(buffer + pos) : 0;
		}

		bool get_bufs(uint8_t* bf, size_t pos, size_t len) {
			if (pos < buf_size && pos + len < buf_size) {
				memcpy(bf, buffer + pos, len);
				return true;
			}
			else {
				return false;
			}
		}
	};

	class WriteBuffer {
		int8_t* buffer;
		size_t initial;
		size_t length;
		size_t raise;
		size_t size;

	protected:
		void initialization() {
			if (buffer) delete[] buffer;
			buffer = new int8_t[initial];
			size   = initial;
			length = 0;
		}

        void __bf_size_check(size_t bf_size) {
            size_t target = length + bf_size;
            if (target >= size) {
                while (target >= (size += raise));
                int8_t* n_buf = new int8_t[size];
                if (length) memcpy(n_buf, buffer, length);
                delete[] buffer;
                buffer = n_buf;
            }
        }

	public:
		WriteBuffer() : WriteBuffer(1024, 500) {}
		WriteBuffer(size_t initial, size_t raise):initial(initial), raise(raise){
			initialization();
		}
		~WriteBuffer() {
			if (buffer) delete[] buffer;
		}

        void clear() {
            initialization();
        }

        void write_int8(int8_t value) {
            __bf_size_check(1);
            buffer[length++] = value;
        }

        void write_int16(int16_t value) {
            write((int8_t *) & value, 2);
        }

        void write_int32(int32_t value) {
            write((int8_t*)&value, 4);
        }

        void write_int64(int64_t value) {
            write((int8_t*)&value, 8);
        }

		void write(const int8_t* bytes, size_t bf_size) {
            __bf_size_check(bf_size);
			memcpy(&buffer[length], bytes, bf_size);
			length = length + bf_size;
		}

        void write_text(const char* text) {
            size_t bf_size = strlen(text);
            if (text && bf_size) {
                write((int8_t*)text, bf_size);
            }
        }

        template<typename... arguments>
        void format_write(const char * fomat, size_t bf_size, arguments... args) {
            __bf_size_check(bf_size);
            char * s_buffer = (char*)&buffer[length];
            sprintf(s_buffer, fomat, args...);
            length = length + strlen(s_buffer);
        }

		void write(const int8_t* bytes, size_t offset, size_t bf_size) {
			write(&bytes[offset], bf_size);
		}

		void rewrite(const int8_t* bytes, size_t pos, size_t bf_size) {
			memcpy(&this->buffer[pos], bytes, bf_size);
		}
        void write_int8(int8_t value, size_t pos) {
            buffer[pos] = value;
        }

        void rewrite_int16(int16_t value, size_t pos) {
            rewrite((int8_t*)&value, pos, 2);
        }

        void rewrite_int32(int32_t value, size_t pos) {
            rewrite((int8_t*)&value, pos, 4);
        }

        void write_int64(int64_t value, size_t pos) {
            rewrite((int8_t*)&value, pos, 8);
        }

		void out_file(const char* file_path) {
			FILE* f_ptr = fopen(file_path, "wb");
			fwrite(this->buffer, 1, length, f_ptr);
			fclose(f_ptr);
		}

		size_t wbf_size() {
			return this->length;
		}

		const int8_t* data() {
			return this->buffer;
		}
	};

    class TextReader {
        struct strline {
            size_t offset;
            size_t length;
        };
    private:
        uint8_t* buffer;
        size_t   lines, size;
        strline* line_array;
        size_t   line_size;
        size_t   max_length;
        char*    out_line;
    protected:
        inline void addline(size_t offset, size_t length) {
            if (!length) return;
            if (!this->line_size || this->lines >= this->line_size) {
                size_t length = this->line_size + 100;
                strline* new_arr = new strline[length];
                if (line_array) {
                    memcpy(new_arr, line_array, sizeof(strline) * line_size);
                    delete[] this->line_array;
                }
                this->line_array = new_arr;
                this->line_size = length;
            }
            max_length = length > max_length ? length : max_length;
            this->line_array[this->lines++] = { offset, length };
        }

        inline void handler(bool line_breaks = true) {
            uint8_t* buf_end  = buffer + size;
            uint8_t* buf_tmp  = buffer;
            uint8_t* current  = buf_tmp;
            uint8_t symbol[3] = "\r\n";
            bool while_finish = false;
            int8_t line_break = 0;
            do {
                if (buf_tmp != (buf_tmp - 1) && *buf_tmp == '\r' 
                    && *(uint16_t*)buf_tmp == *(uint16_t*)symbol) {
                    line_break = 2, buf_tmp += 1;
                }
                else if (*buf_tmp == '\n' || *buf_end == '\r') {
                    line_break = 1;
                }
                while_finish = ++buf_tmp == buf_end;
                if (line_break || while_finish) {
                    uint32_t length = buf_tmp - current;
                    uint32_t offset = current - buffer;
                    length -= line_breaks ? 0 : line_break;
                    this->addline(offset, length);
                    line_break = 0;
                    current = buf_tmp;
                }
            } while (!while_finish);
            if (max_length) {
                this->out_line = new char[max_length + 1];
            }
        }

        void reset() {
            if (this->buffer)     delete[] buffer;
            if (this->out_line)   delete[] out_line;
            if (this->line_array) delete[] line_array;
            this->line_array = nullptr;
            this->out_line   = nullptr;
            this->buffer     = nullptr;
            this->max_length = 0;
            this->line_size  = 0;
            this->lines = 0;
            this->size  = 0;
        }
    public:

        TextReader() { this->reset(); }
        ~TextReader() { this->reset(); }

        TextReader& reader(FILE* file, bool line_breaks = true) {
            this->reset();
            if (file) {
                fseek(file, 0, SEEK_END);
                this->size = ftell(file);
                if (!this->size) return *this;
                this->buffer = new uint8_t[this->size];
                fseek(file, 0, SEEK_SET);
                fread(this->buffer, 1, this->size, file);
                fclose(file);
                this->handler(line_breaks);
            }
            return *this;
        }

        TextReader& reader(const char* filepath, bool line_breaks = false) {
            return this->reader(fopen(filepath, "rb"), line_breaks);
        }

        bool has_data() {
            return this->size && this->buffer;
        }

        const size_t bf_size() {
            return this->size;
        }

        const size_t count() {
            return this->lines;
        }

        const uint8_t* bf_get() {
            return this->buffer;
        }

        const char* get_line(size_t num) {
            strline* line_tmp = &this->line_array[num];
            uint8_t* tar_ptr = &this->buffer[line_tmp->offset];
            memcpy(this->out_line, tar_ptr, line_tmp->length);
            this->out_line[line_tmp->length] = '\0';
            return this->out_line;
        }

        template <typename funcallback>
        void foreach(funcallback handler) {
            for (size_t i = 0; i < this->lines; i++) {
                handler(i, (char*)this->get_line(i));
            }
        }
    };
}