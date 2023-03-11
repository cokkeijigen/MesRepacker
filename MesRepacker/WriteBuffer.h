#pragma once

class WriteBuffer {
	byte* buffer;
	size_t raise;
	long initial, size, len;

public:
	WriteBuffer() : WriteBuffer(1024, 500) {}

	WriteBuffer(size_t initial, size_t raise) {
		this->initial = initial;
		this->raise = raise;
		this->reset();
	}

	void reset() {
		if (this->buffer) delete[] this->buffer;
		this->buffer = new byte[this->initial];
		this->size = this->initial;
		this->len = 0;
	}

	void Oversize(int b_len) {
		int target = this->len + b_len;
		if (target < this->size) return;
		while (target > (this->size += this->raise));
		byte* tmp = new byte[this->size];
		if (len) memcpy(tmp, this->buffer, this->len + 1);
		delete[] this->buffer;
		this->buffer = tmp;
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

	long lensub(int sub = 0) {
		return this->len - sub;
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