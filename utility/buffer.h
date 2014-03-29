#ifndef __BUFFER__
#define __BUFFER__

class Buffer {
public:

	Buffer();
	Buffer(int length);
	Buffer(const Buffer& buffer);

	~Buffer();

	void set_length_part();
	void set_count_part(USHORT func_count);

	int get_length_part();
	USHORT get_count_part();

	char* get_buffer();
	int get_size();
	void clear();

	//go back to last block of data
	void go_back(char* sv_ptr);

	char* get_cur_ptr();
	char* get_cur_ptr(int length);
	void move_over(int length);

	int read_int();
	UINT read_uint();
	char read_char();
	UCHAR read_uchar();
	short read_short();
	USHORT read_ushort();
	float read_float();
	void read_byte_arr(char* dst, int length);


	void write_int(int);
	void write_uint(UINT);
	void write_char(char);
	void write_uchar(UCHAR);
	void write_short(short);
	void write_ushort(USHORT);
	void write_float(float);
	void write_byte_arr(char* src, int length);

	char* com_buffer_;
	char* buffer_;

protected:
	int capacity_;
	

	int size_;
	char* cur_ptr;
};

#endif
