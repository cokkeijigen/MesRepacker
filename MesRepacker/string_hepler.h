#pragma once

namespace StringHelper {

	struct Charset {
		char name[15];
		uint8_t(*getlen)(char);
		uint32_t codepage;
	};

	struct character {
		uint64_t data = NULL;
	};

	character to_character(uint64_t value) {
		uint8_t  size = *(((char*)&value) + 7) ? 8 : (uint8_t)strlen((char*)&value);
		uint64_t c_Val = 0x0i64;
		for (uint8_t i = 0; i < size; i++) {
			c_Val |= ((value >> (8 * i)) & 0xFF) << (8 * (size - i - 1));
		}
		return { c_Val };
	}

	character to_character(const char* value) {
		uint32_t length = strlen(value);
		if (length > 8) {
			throw std::runtime_error("Length too long! Convert to character failed.");
		}
		uint64_t c_Val = 0x0i64;
		memcpy(&c_Val, value, length);
		return { c_Val };
	}

	class Characters {
		friend class String;
		friend std::ostream& operator<< (std::ostream&, StringHelper::Characters&);
		friend bool operator==(StringHelper::Characters, StringHelper::Characters);
		friend bool operator!=(StringHelper::Characters, StringHelper::Characters);
	private:
		uint8_t  size;
		intptr_t str;
		bool is_created;
	public:
		Characters() : Characters(0, 0) {
		}
		Characters(intptr_t str, uint8_t size) : str(str), size(size), is_created(false) {
		}
		Characters(const Characters& cchar) : Characters(cchar.str, cchar.size) {
		}
		Characters(character cchar) : is_created(true) {
			if (*(((char*)&cchar.data) + 7)) {
				this->str = (intptr_t)new char[9] { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
				memcpy(&cchar.data, (char*)str, 8);
				this->size = 8;
			}
			else {
				uint8_t size = (uint8_t)strlen((char*)&cchar.data);
				this->str = (intptr_t) new char[size + 1];
				strcpy((char*)this->str, (char*)&cchar.data);
				this->size = size;
			}
		}
		Characters(const char* str) : is_created(true) {
			if (str) {
				uint32_t size = strlen(str);
				if (size > 255) {
					throw std::runtime_error("Length too long! Maximum length is 255.");
				}
				else {
					this->str = (intptr_t) new char[size + 1];
					strcpy((char*)this->str, str);
					this->size = size;
				}
			}
			else {
				this->size = 0;
				this->str = 0;
			}
		}
		~Characters() {
			if (is_created && this->str) {
				delete[](char*)this->str;
			}
		}

		uint8_t get_size() {
			return this->size;
		}

		const char* to_c_str() {
			if (!this->is_created) {
				char* bf = new char[size + 1];
				memcpy(bf, (char*)str, size);
				bf[size] = '\0';
				str = (intptr_t)bf;
				is_created = true;
			}
			return (const char*)str;
		}

		character to_character() {
			if (size > 8) throw std::runtime_error("Length too long! Convert to character failed.");
			uint64_t result = 0x0i64;
			memcpy(&result, (char*)str, size);
			return { result };
		}

		bool isEmpty() {
			return !(size && str);
		}
	};

	class IndexPage {
		friend class String;
		struct page {
			uint8_t* array  = NULL;
			uint8_t  length = NULL;
			uint8_t  size   = NULL;
		};
	protected:
		page* page_arr;
		uint32_t arr_len;
		uint32_t page_num;
		uint32_t size;

	private:
		void __get_val(page* const _page, int32_t& _res, uint32_t _val) {
			for (uint32_t i = 0; i < _page->length; i++) {
				if (_page->array[i] == _val) {
					_res = i;
					break;
				}
			}
		}

	public:
		IndexPage(uint32_t _arr_len = 4) : arr_len(_arr_len) {
			page_arr = new page[arr_len];
			page_num = 0;
			size = 0;
		}
		~IndexPage() {
			this->clear(true);
		}

		void clear(bool _delete = false) {
			if (page_arr) {
				for (uint32_t i = 0; i < arr_len; i++) {
					page* crtpg = &page_arr[page_num];
					if (crtpg->array) delete[] crtpg->array;
					crtpg->array = NULL;
					crtpg->size = NULL;
					crtpg->length = NULL;
				}
				if (_delete) {
					delete[] page_arr;
					page_arr = 0;
					arr_len = 0;
				}
			}
			page_num = NULL;
			size = 0;
		}

		void add(uint32_t value) {
		_start:
			if (page_num >= arr_len) {
				uint32_t n_arr_len = arr_len + 4;
				page* n_page_arr = new page[n_arr_len]{ 0, 0, 0 };
				if (page_arr && arr_len) {
					memcpy(n_page_arr, page_arr, sizeof(page) * arr_len);
					delete[] page_arr;
				}
				arr_len  = n_arr_len;
				page_arr = n_page_arr;
			}
			page* crtpg = &page_arr[page_num];
			if (value > (255 * (page_num + 1))) {
			_add_page:
				uint8_t len = crtpg->length;
				if (len && len < crtpg->size && crtpg->array) {
					uint8_t* n_buf = new uint8_t[len];
					memcpy(n_buf, crtpg->array, len);
					delete[] crtpg->array;
					crtpg->array = n_buf;
					crtpg->size  = len;
				}
				++page_num;
				goto _start;
			}
			if (!crtpg->array) {
				crtpg->array = new uint8_t[255];
				crtpg->size  = 255;
			}
			if (crtpg->array && crtpg->length < crtpg->size) {
				crtpg->array[crtpg->length++] = value - (page_num * 255);
			}
			else {
				goto _add_page;
			}
			size++;
		}

		int32_t get_offset(uint32_t index) {
			for (uint32_t i = 0; i < arr_len; i++) {
				page* crtpg = &page_arr[i];
				if (index > crtpg->length) {
					index -= crtpg->length;
				}
				else {
					uint32_t val = crtpg->array[index];
					return i > 0 ? (val += 255 * i) : val;
				}
			}
			return -1;
		}

		int32_t get_index(uint32_t position) {
			uint32_t n_page = position / 255;
			int32_t result = -1;
			if (n_page) {
				__get_val(&page_arr[n_page], result, position % 255);
				if (result != -1) {
					for (uint32_t i = 0; i < n_page; i++) {
						result += page_arr[i].length;
					}
				}
			}
			else {
				__get_val(page_arr, result, position);
			}
			return result;
		}

		uint32_t length() {
			return this->size;
		}
	};

	class String {
		friend std::ostream& operator<< (std::ostream&, StringHelper::String&);
		struct cbuffer {
			char* buffer = NULL;
			uint32_t length = NULL;
			uint32_t size = NULL;
		};

	protected:
		bool page_created = false;
		bool is_default = false;
		IndexPage index_page;
		uint32_t  crtpos = 0;
		uint32_t  length = 0;
		Charset   charset;
		cbuffer   buffer;

	private:
		void __strbfcopy(const char* strbf = NULL, uint32_t size = 0) {
			if (strbf) {
				if (!buffer.buffer) {
					buffer.size = strlen(strbf) + 1;
					if (size && size > buffer.size) {
						buffer.size = size;
					}
					buffer.buffer = new char[buffer.size];
					strcpy(buffer.buffer, strbf);
				}
				else {
					uint32_t c_bfsize = strlen(buffer.buffer);
					uint32_t i_bfsize = strlen(strbf);
					if (c_bfsize + i_bfsize >= buffer.size) {
						if (size + buffer.size > buffer.size + i_bfsize) {
							buffer.size += size;
						}
						else {
							buffer.size += i_bfsize;
						}
						char* n_buffer = new char[buffer.size];
						strcpy(n_buffer, buffer.buffer);
						if (strbf < buffer.buffer || strbf > buffer.buffer + buffer.size) {
							delete[] buffer.buffer;
						}
						buffer.buffer = n_buffer;
					}
					if (buffer.length) {
						strcat(buffer.buffer, strbf);
					}
					else {
						strcpy(buffer.buffer, strbf);
					}
				}
				buffer.length = strlen(buffer.buffer);
			}
			else {
				if (!buffer.buffer) {
					buffer.length = 0;
					buffer.size = size ? size : 1024;
					buffer.buffer = new char[buffer.size];
					memset(buffer.buffer, 0, buffer.size);
				}
				else {
					uint32_t nbf_size = buffer.size + (size ? size : 1024);
					char* nbf_ptr     = new char[nbf_size];
					memcpy(nbf_ptr, buffer.buffer, buffer.size);
					delete[] buffer.buffer;
					buffer.buffer = nbf_ptr;
					buffer.size   = nbf_size;
				}
			}
		}

		uint32_t __index_page_creater() {
			if(page_created) goto _result;
			if (is_default) {
				length = strlen(buffer.buffer);
			}
			else if (buffer.buffer) {
				uint32_t sbfsize = strlen(buffer.buffer);
				if (!sbfsize) return length;
				uint32_t i = crtpos;
				while (i < sbfsize) {
					uint8_t c_len = charset.getlen(buffer.buffer[i]);
					index_page.add(i);
					crtpos = (i += c_len);
					if (++length == INT32_MAX) {
						throw std::runtime_error("Length too long!");
					}
				}
				page_created = true;
			}
		_result:
			return length;
		}

		bool __contains(String& i_str) {
			if (!page_created) __index_page_creater();
			uint32_t i = 0, v1 = 0, v2 = 0;
			while (i < this->length) {
				if (i_str.at(0) == this->at(i)) {
					bool eq = true;
					for (v1 = 1, v2 = i + 1; v1 < i_str.length; v1++, v2++) {
						if (i_str.at(v1) != this->at(v2)) {
							eq = false;
							break;
						}
					}
					if (eq) return true;
				}
				i++;
			}
			return false;
		}

		bool __start_with(String& i_str) {
			if (i_str.charset.codepage != this->charset.codepage || !i_str.length)
				return false;
			if (!page_created) __index_page_creater();
			for (uint32_t i = 0; i < i_str.length; i++) {
				if (this->at(i) != i_str.at(i)) return false;
			}
			return true;
		}

		bool __end_with(String& i_str) {
			if (i_str.charset.codepage != this->charset.codepage || !i_str.length)
				return false;
			if (!page_created) __index_page_creater();
			uint32_t start = this->length - i_str.length;
			for (uint32_t i = start, j = 0; start < this->length; i++, j++) {
				if (this->at(i) != i_str.at(j)) return false;
			}
			return true;
		}

		bool __equals(String& i_str) {
			if ((!i_str.length || this->length) || this->length != i_str.length)
				return false;
			if (i_str.charset.codepage != this->charset.codepage)
				return false;
			for (uint32_t i = 0; i < length; i++) {
				if (this->at(i) != i_str.at(i))
					return false;
			}
			return true;
		}

		void __init_page() {
			this->page_created = false;
			this->index_page.clear();
			this->length = 0;
			this->crtpos = 0;
		}

	public:
		String(const char* str = NULL) : is_default(true) {
			__strbfcopy(str);
			this->charset = {
				"Default",
				[](char c) -> uint8_t { return 1; },
				0
			};
		}
		String(const String& str) : String(str.buffer.buffer, str.charset) {
			this->is_default = str.is_default;
		}
		String(Charset charset) : String(0, charset) {
		}
		String(const char* str, Charset charset) {
			__strbfcopy(str);
			this->charset = charset;
		}
		~String() { this->clear(); }

		String& operator=(String str) {
			this->charset = str.charset;
			return this->assign(str.buffer.buffer);
		}

		uint32_t code_page() {
			return charset.codepage;
		}

		uint32_t get_length() {
			return is_default ? buffer.length : page_created ? length : __index_page_creater();
		}

		Charset get_charset() {
			return this->charset;
		}

		const char* get_charset_name() const {
			return this->charset.name;
		}

		const char* get_c_str() const {
			return this->buffer.buffer;
		}

		uint32_t get_c_strlen() {
			return this->buffer.length;
		}

		void change_charset(Charset charset) {
			this->charset = charset;
			this->page_created = false;
			crtpos = 0, length = 0;
			index_page.clear();
		}

		void clear() {
			if (buffer.buffer) {
				delete[] buffer.buffer;
			}
			buffer.length = 0;
			buffer.buffer = 0;
			buffer.size   = 0;
			__init_page();
		}

		String& remove(String& str) {
			return this->remove(str.buffer.buffer);
		}

		String& remove(const char* str) {
			return this->replace(str, NULL);
		}

		String& replace(String& oldstr, String& newstr) {
			return replace(oldstr.buffer.buffer, newstr.buffer.buffer);
		}

		String& replace(const char* oldstr, const char* newstr) {
			if (!oldstr || !buffer.buffer || !buffer.length) return *this;
			uint32_t oldstrlen  = strlen(oldstr);
			uint32_t newstrlen  = newstr ? strlen(newstr) : 0;
			uint32_t difference = newstrlen - oldstrlen;
			uint32_t currentlen = buffer.length;
			char* find_start    = buffer.buffer;
			bool need_init_page = false;
			while (true) {
				char* find = strstr(find_start, oldstr);
				if (find) {
					if (newstrlen > oldstrlen && currentlen + newstrlen >= buffer.size) {
						uint32_t offset = find - buffer.buffer;
						if (newstrlen > 1024) {
							__strbfcopy(0, newstrlen);
						}
						else {
							__strbfcopy();
						}
						find = buffer.buffer + offset;
						find_start = find + newstrlen;
					}
					if (newstrlen != 0) {
						memmove(
							find + newstrlen,
							find + oldstrlen,
							strlen(find + oldstrlen) + 1
						);
						memcpy(find, newstr, newstrlen);
					}
					else
					{
						strcpy(find, find + oldstrlen);
					}
					currentlen += difference;
					need_init_page = true;
				}
				else break;
			}
			if (need_init_page) {
				buffer.buffer[currentlen] = '\0';
				__init_page();
			}
			return *this;
		}

		String& strips(const char* str = NULL) {
			if (str) {
				uint32_t s_len = strlen(str);
				uint32_t bf_len = this->buffer.length;
				char* start = this->buffer.buffer;
				char* end = start + bf_len - 1;
				bool is_start = false, is_end = false;
				while (true) {
					if (!is_start && this->start_with(str)) {
						buffer.buffer += s_len;
					}
					else if (!is_start) {
						is_start = true;
					}
					if (!is_end && this->end_with(str)) {
						*(end -= s_len) = '\0';
					}
					else if (!is_end) {
						is_end = true;
					}
					if (is_start && is_end) {
						break;
					}
				}
				bool need_init_page = false;
				if (start != this->buffer.buffer) {
					strcpy(start, this->buffer.buffer);
					this->buffer.buffer = start;
					need_init_page = true;
				}
				else if (end != start + bf_len - 1) {
					need_init_page = true;
				}
				buffer.length = strlen(this->buffer.buffer);
				if (need_init_page) __init_page();
			}
			return this->trim();
		}

		String& trim() {
			uint32_t c_len = this->buffer.length;
			char* start = this->buffer.buffer;
			char* end = start + c_len - 1;
			bool is_start = false, is_end = false;
			while (!(is_start && is_end) && start <= end) {
				if (!is_start && *start != 0x20 && *start != '\n' && *start != '\r') {
					is_start = true;
				}
				else if (!is_start) start++;
				if (!is_end && *end != 0x20 && *end != '\n' && *end != '\r') {
					is_end = true;
				}
				else if (!is_end) end--;
			}
			bool need_init_page = false;
			if (*(end + 1)) *(end + 1) = '\0';
			if (start != this->buffer.buffer) {
				strcpy(this->buffer.buffer, start);
				need_init_page = true;
			}
			else if (end != this->buffer.buffer + c_len - 1) {
				need_init_page = true;
			}
			buffer.length = strlen(buffer.buffer);
			if (need_init_page) __init_page();
			return *this;
		}

		String& commit() {
			uint32_t bfsize = strlen(buffer.buffer);
			if (bfsize + 1 != this->buffer.size) {
				char* n_buffer = new char[bfsize + 1];
				strcpy(n_buffer, this->buffer.buffer);
				delete[] this->buffer.buffer;
				this->buffer.buffer = n_buffer;
				this->buffer.length = bfsize;
				this->buffer.size = bfsize;
				__init_page();
			}
			__index_page_creater();
			return *this;
		}

		String& append(String& str) {
			if (&str == this) {
				char* bf = str.buffer.buffer;
				this->append(str.buffer.buffer);
				delete[] bf;
			}
			else {
				this->append(str.buffer.buffer);
			}
			return *this;
		}

		String& append(const char* str) {
			if (str) {
				this->page_created = false;
				__strbfcopy(str);
			}
			return *this;
		}

		String& append(Characters cchar) {
			if (!cchar.str || !*(char*)cchar.str) {
				return *this;
			}else if (!cchar.is_created) {
				char* str = (char*)cchar.str;
				char c = str[cchar.size];
				if (c) {
					str[cchar.size] = '\0';
					this->append(str);
					str[cchar.size] = c;
				}
				else {
					this->append(str);
				}
				return *this;
			}
			return this->append((char*)cchar.str);
		}

		String& assign(String& str) {
			this->charset = str.charset;
			this->is_default = str.is_default;
			return this->assign(str.buffer.buffer);
		}

		String& assign(Characters& cchar) {
			char* str = (char*)cchar.str;
			char c = str[cchar.size];
			if (!cchar.is_created && c) {
				str[cchar.size] = '\0';
				return this->assign(str);
				str[cchar.size] = c;
			}
			else {
				return this->assign(str);
			}
		}

		String& assign(const char* str) {
			this->clear();
			__strbfcopy(str);
			return *this;
		}

		Characters at(int32_t index) {
			if (is_default) {
				return Characters((intptr_t)(buffer.buffer + index), 1);
			}
			if (!page_created || !this->index_page.size) {
				__index_page_creater();
			}
			if (this->index_page.size) {
				index = index < 0 ? length - (index *= -1) : index;
				int32_t offset = index_page.get_offset(index);
				if (offset != -1 && (uint32_t)offset < buffer.length) {
					char* str = buffer.buffer + offset;
					return Characters((intptr_t)str, charset.getlen(*str));
				}
			}
			return Characters(0);
		}

		String substrs(int32_t start, int32_t length) {
			return this->substr(start, start + length);
		}

		String substr(int32_t i_start = 0, int32_t i_end = 0) {
			String result(this->charset);
			if (i_end == length) i_end = 0;
			uint32_t start = i_start < 0 ? length - (i_start *= -1) : i_start;
			uint32_t end = i_end < 0 ? length - (i_end *= -1) : i_end;
			if (start < length && end < length) {
				if (!end && !start) {
					result.assign(buffer.buffer);
				}
				else {
					char* s_start = buffer.buffer, * s_end = 0;
					if (start > 0 && end > 0 && end > start) {
						s_start = (char*)this->at(start).str;
						s_end = (char*)this->at(end).str;
					}
					else if (!start && end > 0) {
						s_end = (char*)this->at(end).str;
					}
					else if (!end && start > 0) {
						s_start = (char*)this->at(start).str;
						s_end = buffer.buffer + buffer.length;
					}
					if (s_end) {
						int32_t len = s_end - s_start;
						if (len > 0) {
							char* tmp = new char[len + 1];
							memcpy(tmp, s_start, len);
							tmp[len] = '\0';
							result.assign(tmp);
							delete[] tmp;
						}
					}
				}
			}
			return result;
		}

		int32_t find(Characters cchar, int index = 0) {
			char* find_start = 0;
			if (index) {
				Characters tmp = this->at(index);
				if (!tmp.isEmpty()) {
					find_start = (char*)tmp.str;
				}
			}
			else {
				find_start = this->buffer.buffer;
			}
			if (find_start) {
				char* findPtr = 0;
				char* str = (char*)cchar.str;
				char c = str[cchar.size];
				if (!c || cchar.is_created) {
					findPtr = strstr(find_start, str);
				}
				else {
					str[cchar.size] = '\0';
					findPtr = strstr(find_start, str);
					str[cchar.size] = c;
				}
				if (findPtr) {
					return index_page.get_index(findPtr - buffer.buffer) - index;
				}
			}
			return -1;
		}

		int32_t find_last_of(String& str) {
			return this->find_last_of(str.buffer.buffer);
		}

		int32_t find_last_of(character& cchar) {
			if (cchar.data) {
				if (!page_created) __index_page_creater();
				for (int32_t i = (int32_t)length - 1; i >= 0; i--) {
					if (this->at(i).to_character().data == cchar.data)
						return i;
				}
			} return -1;
		}

		int32_t find_last_of(const char* str) {
			int findlen = strlen(str);
			int bfrlen = strlen(this->buffer.buffer);
			char* current = this->buffer.buffer + bfrlen - findlen;
			while (current != this->buffer.buffer) {
				char* findPtr = strstr(current, str);
				if (findPtr) {
					if (!page_created) __index_page_creater();
					return index_page.get_index(findPtr - this->buffer.buffer);
				}
				current--;
			}
			return -1;
		}

		int32_t find_frist_of(String& str) {
			return this->find_frist_of(str.buffer.buffer);
		}

		int32_t find_frist_of(character& cchar) {
			if (cchar.data) {
				if (!page_created) __index_page_creater();
				for (uint32_t i = 0; i < length; i++) {
					if (this->at(i).to_character().data == cchar.data)
						return i;
				}
			} return -1;
		}

		int32_t find_frist_of(const char* str) {
			char* findPtr = strstr(this->buffer.buffer, str);
			if (findPtr) {
				if (!page_created) __index_page_creater();
				return index_page.get_index(findPtr - this->buffer.buffer);
			}
			return -1;
		}

		bool contains(Characters& cchar) {
			return this->contains((character&)cchar.to_character());
		}

		bool contains(character& cchar) {
			if (cchar.data) {
				if (!page_created) __index_page_creater();
				for (uint32_t i = 0; i < this->length; i++) {
					if (this->at(i).to_character().data == cchar.data) {
						return true;
					}
				}
			}
			return false;
		}

		bool contains(String& str, bool strict_mode = false) {
			if (strict_mode) {
				if (!str.length) return false;
				return __contains(str);
			}
			else {
				return this->contains(str.buffer.buffer);
			}
		}

		bool contains(const char* str, bool strict_mode = false) {
			uint32_t size = strlen(str);
			if (size && buffer.buffer && buffer.length) {
				if (strict_mode) {
					return __contains((String&)String(str, charset));
				}
				else {
					return strstr(buffer.buffer, str);
				}
			}
			return false;
		}

		bool start_with(character& cchar) {
			if (cchar.data) {
				return this->at(0).to_character().data == cchar.data;
			} return false;
		}

		bool start_with(String& str, bool strict_mode = false) {
			if (strict_mode) return __start_with(str);
			else return this->start_with(str.buffer.buffer);
		}

		bool start_with(const char* str, bool strict_mode = false) {
			uint32_t size = strlen(str);
			if (size && buffer.buffer && size < buffer.length) {
				if (strict_mode) {
					return __start_with((String&)String(str, charset));
				}
				else if (buffer.buffer) {
					char* bf_ptr = this->buffer.buffer;
					while (true) {
						if (!*str || !*bf_ptr) break;
						if (*str++ != *bf_ptr++) return false;
					}
					return true;
				}
			}
			return false;

		}

		bool end_with(character& cchar) {
			if (cchar.data) {
				return this->at(-1).to_character().data == cchar.data;
			} return false;
		}

		bool end_with(String& str, bool strict_mode = false) {
			if (strict_mode) return __end_with(str);
			else return this->end_with(str.buffer.buffer);
		}

		bool end_with(const char* str, bool strict_mode = false) {
			uint32_t size = strlen(str);
			if (size && buffer.buffer && size < buffer.length) {
				if (strict_mode) {
					return __end_with((String&)String(str, charset));
				}
				else if (buffer.buffer) {
					buffer.length = strlen(buffer.buffer);
					if (buffer.length) {
						char* bf_ptr = buffer.buffer + (buffer.length - size);
						while (true) {
							if (!*str || !*bf_ptr) break;
							if (*str++ != *bf_ptr++) return false;
						}
						return true;
					}
				}
			}
			return false;
		}

		bool equals(const char* str, bool strict_mode = false) {
			uint32_t size = strlen(str);
			if (size && buffer.buffer && size == buffer.length) {
				if (strict_mode) {
					return __equals((String&)String(str, charset));
				}
				else {
					char* bf_ptr = buffer.buffer;
					while (true) {
						if (!*str || !*bf_ptr) break;
						if (*str++ != *bf_ptr++) return false;
					}
					return true;
				}
			}
			return false;
		}

		bool is_empty() {
			return !(commit().length && buffer.length);
		}
	};

	class UTF8String : public String {
		void change_charset(Charset charset) {};
	public:
		UTF8String(const char* str = 0) : String(str, {
			"UTF-8",
			[](char c) -> uint8_t {
				return (c & (char)128) ? (c & (char)32) ? (c & (char)16) ? 4 : 3 : 2 : 1;
		}, 65001 }) {}
		UTF8String(const String& str) : UTF8String(str.get_c_str()) {
		}

		UTF8String& assign(String& str) {
			this->assign(str.get_c_str());
			return *this;
		}

		UTF8String& assign(Characters& cchar) {
			String::assign(cchar);
			return *this;
		}

		UTF8String& assign(const char* str) {
			String::assign(str);
			return *this;
		}

		UTF8String& operator=(String str) {
			return this->assign(str.get_c_str());
		}
	};

	std::ostream& operator<< (std::ostream& cout, StringHelper::String& str) {
		return str.buffer.length > 0 ? cout << str.buffer.buffer : cout;
	}

	std::ostream& operator<< (std::ostream& cout, StringHelper::String* str) {
		return StringHelper::operator<<(cout, *str);
	}

	std::ostream& operator<< (std::ostream& cout, StringHelper::character cchar) {
		if (*(((char*)&cchar.data) + 7)) {
			char str[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			*(uint64_t*)str = cchar.data;
			return cout << str;
		}
		else {
			return cchar.data ? cout << ((char*)(&cchar.data)) : cout;
		}
	}

	std::ostream& operator<< (std::ostream& cout, StringHelper::Characters& cchar) {
		if (cchar.size && cchar.str) {
			if (cchar.is_created) {
				cout << (char*)cchar.str;
			}
			else {
				char* str = (char*)cchar.str;
				char c = str[cchar.size];
				if (!cchar.is_created && c) {
					str[cchar.size] = '\0';
					cout << str;
					str[cchar.size] = c;
				}
				else {
					cout << str;
				}
			}
		}
		return cout;
	}

	std::ostream& operator<< (std::ostream& cout, StringHelper::Characters* cchar) {
		return StringHelper::operator<<(cout, *cchar);
	}

	bool operator == (StringHelper::character cchar1, StringHelper::character cchar2) {
		return cchar1.data == cchar2.data;
	}

	bool operator==(StringHelper::Characters cchar1, StringHelper::Characters cchar2) {
		if (cchar1.str == cchar2.str && cchar1.size == cchar2.size) {
			return true;
		}
		else if (cchar1.size == cchar2.size) {
			uint8_t i = 0;
			char* ch1 = (char*)cchar1.str;
			char* ch2 = (char*)cchar2.str;
			while (i != cchar1.size) {
				if (*ch1 != *ch2) return false;
				ch1++, ch2++, i++;
			}
			return true;
		}
		return false;
	}

	bool operator!=(StringHelper::Characters cchar1, StringHelper::Characters cchar2) {
		return !StringHelper::operator==(cchar1, cchar2);
	}
}

#define _STRINGHELPER_H
#define sstr StringHelper::String
#define utf8str StringHelper::UTF8String
#define cchar StringHelper::Characters
#define charc StringHelper::character
#define to_charc(val) StringHelper::to_character(val)
#define sstr_new(str, c_set) StringHelper::String((const char*)str, c_set)
#define sstr_new_ptr(str, c_set) new StringHelper::String((const char*)str, c_set)
#define utf8str_new(str) StringHelper::UTF8String((const char*)str)
#define utf8str_new_ptr(str) new StringHelper::UTF8String((const char*)str)
#define delete_ptr(_ptr) delete _ptr