#ifndef __COMPRESSOR__
#define __COMPRESSOR__

#include <windows.h>

class Compressor {
public:
	static void encode_uint(UINT value, char*& buffer);
	static void encode_int(int value, char*& buffer);
	static void decode_uint(char*& buffer, UINT& value);
	static void decode_int(char*& buffer, int& value);

	static void print_num(int num);
	static void print_buf(char* buf, int size);

	static bool lzo_encode(char* src_buf, int inlen, char* dst_buf, int& outlen);
	static bool lzo_decode(char* src_buf, int inlen, char* dst_buf, int& outlen);
};


#endif