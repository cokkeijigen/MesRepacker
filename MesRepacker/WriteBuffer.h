#pragma once

class WriteBuffer {
	byte* buffer;
	size_t raise;
	long initial, size, len;

public:
	WriteBuffer() : WriteBuffer(1024, 500) {}

	WriteBuffer(size_t initial, size_t raise) {
		this->buffer = new byte[initial];
		this->initial = initial;
		this->size = initial;
		this->raise = raise;
		this->len = 0;
	}

	void reset() {
		if (this->buffer) delete[] this->buffer;
		this->buffer = new byte[this->initial];
		this->size = this->initial;
		this->len = 0;
	}

	void Oversize(int b_len) {
		if (this->len + b_len >= size) {
			size += raise;
			byte* tmp = new byte[size];
			if (len) memcpy(tmp, this->buffer, this->len + 1);
			delete[] this->buffer;
			this->buffer = tmp;
			if (this->len + b_len >= size) Oversize(b_len);
		}
	}

	void write(byte _byte) {
		Oversize(1);
		this->buffer[this->len++] = _byte;
	}

	void write(byte *bytes, int b_len) {
		Oversize(b_len);
		memcpy(&this->buffer[this->len], bytes, b_len);
		this->len += b_len;
	}

	void rewrite(byte* bytes, long pos, int b_len) {
		memcpy(&this->buffer[pos], bytes, b_len);
	}

	void write(ReadBuffer* rf, long pos, int b_len) {
		byte* tmp = new byte[b_len];
		rf->get(tmp, pos, b_len);
		this->write(tmp, b_len);
		delete[] tmp;
	}

	long lenf() {
		return len;
	}

	byte* fbuf() {
		return this->buffer;
	}

	void outFile(const char* filepath) {
		FILE* fp = fopen(filepath, "wb");
		fwrite(this->buffer, 1, this->len, fp);
		fclose(fp);
	}

	~WriteBuffer() {
		if (this->buffer) delete[] this->buffer;
	}
};