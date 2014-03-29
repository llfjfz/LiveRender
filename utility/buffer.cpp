#include "utility.h"

Buffer::Buffer(int capacity): capacity_(capacity) {
	buffer_ = new char[capacity_ + 105];
	com_buffer_ = new char[capacity_ + 105];

	cur_ptr = buffer_ + 4;
}

Buffer::Buffer(): capacity_(MAXPACKETSIZE) {
	buffer_ = new char[capacity_ + 105];
	com_buffer_ = new char[capacity_ + 105];

	cur_ptr = buffer_ + 4;
}

Buffer::Buffer(const Buffer& other) {
	printf("Buffer::Buffer(const Buffer&) called\n");
	capacity_ = other.capacity_;
	buffer_ = new char[other.capacity_ + 105];
	com_buffer_ = new char[other.capacity_ + 105];
	memcpy(buffer_, other.buffer_, other.capacity_);
	cur_ptr = buffer_ + 4;
}

Buffer::~Buffer() {
	if(buffer_) {
		delete[] buffer_;
		buffer_ = NULL;
	}

	if(com_buffer_) {
		delete[] com_buffer_;
		com_buffer_ = NULL;
	}
}

void Buffer::clear() {
	cur_ptr = buffer_ + 4;
}

void Buffer::move_over(int length) {
	cur_ptr += length;
}

char* Buffer::get_cur_ptr(int length) {
	char* ptr = cur_ptr;
	cur_ptr += length;
	return ptr;
}

char* Buffer::get_cur_ptr() {
	return cur_ptr;
}

void Buffer::set_length_part() {
	size_ = cur_ptr - buffer_;
	memcpy(buffer_, &size_, sizeof(int));
}

int Buffer::get_length_part() {
	int ret = *( (int*)(buffer_) );
	return ret;
}

void Buffer::set_count_part(USHORT func_count) {
	memcpy(buffer_ + 4, &func_count, sizeof(short));
}

USHORT Buffer::get_count_part() {
	USHORT ret = *( (USHORT*)(buffer_ + 4) );
	return ret;
}

char* Buffer::get_buffer() {
	return buffer_;
}

int Buffer::get_size() {
	return size_ = (cur_ptr - buffer_);
}

void Buffer::go_back(char* sv_ptr) {
	cur_ptr = sv_ptr;
}

void Buffer::write_int(int data) {

#ifdef USE_VARINT
	Compressor::encode_int(data, cur_ptr);
#else
	*( (int*)(cur_ptr) ) = data;
	cur_ptr += sizeof(int);
#endif
}

void Buffer::write_uint(UINT data) {
#ifdef USE_VARINT
	Compressor::encode_uint(data, cur_ptr);
#else
	*( (UINT*)(cur_ptr) ) = data;
	cur_ptr += sizeof(UINT);
#endif
}

void Buffer::write_char(char data) {
	*( (char*)(cur_ptr) ) = data;
	cur_ptr += sizeof(char);
}

void Buffer::write_uchar(unsigned char data) {
	*( (unsigned char*)(cur_ptr) ) = data;
	cur_ptr += sizeof(unsigned char);
}

void Buffer::write_float(float data) {
	*( (float*)(cur_ptr) ) = data;
	cur_ptr += sizeof(float);
}

void Buffer::write_short(short data) {
	*( (short*)(cur_ptr) ) = data;
	cur_ptr += sizeof(short);
}

void Buffer::write_ushort(USHORT data) {
	*( (USHORT*)(cur_ptr) ) = data;
	cur_ptr += sizeof(USHORT);
}

void Buffer::write_byte_arr(char* data, int length) {
	memcpy(cur_ptr, data, length);
	cur_ptr += length;
}

int Buffer::read_int() {
#ifdef USE_VARINT
	int data;
	Compressor::decode_int(cur_ptr, data);
	return data;
#else
	int data = *( (int*)(cur_ptr) );
	cur_ptr += sizeof(int);
	return data;
#endif
}

UINT Buffer::read_uint() {
#ifdef USE_VARINT
	UINT data;
	Compressor::decode_uint(cur_ptr, data);
	return data;
#else
	UINT data = *( (UINT*)(cur_ptr) );
	cur_ptr += sizeof(UINT);
	return data;
#endif
}

char Buffer::read_char() {
	char data = *( (char*)(cur_ptr) );
	cur_ptr += sizeof(char);
	return data;
}

UCHAR Buffer::read_uchar() {
	UCHAR data = *( (UCHAR*)(cur_ptr) );
	cur_ptr += sizeof(UCHAR);
	return data;
}

float Buffer::read_float() {
	float data = *( (float*)(cur_ptr) );
	cur_ptr += sizeof(float);
	return data;
}

short Buffer::read_short() {
	short data = *( (short*)(cur_ptr) );
	cur_ptr += sizeof(short);
	return data;
}

USHORT Buffer::read_ushort() {
	USHORT data = *( (USHORT*)(cur_ptr) );
	cur_ptr += sizeof(USHORT);
	return data;
}

void Buffer::read_byte_arr(char* dst, int length) {
	memcpy(dst, cur_ptr, length);
	cur_ptr += length;
}


