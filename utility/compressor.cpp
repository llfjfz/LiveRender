#include "compressor.h"
#include <algorithm>
using namespace std;
#include <malloc.h>

#include "lzo/minilzo.h"
#define MAX_LEN 3024000

#define OUT_LEN(x) ( x+x/16+64+3)
static float __LZO_MMODEL *out;

#define HEAP_ALLOC(var,size) \
	lzo_align_t __LZO_MMODEL var [ (( size )+ (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem,LZO1X_1_MEM_COMPRESS);


bool Compressor::lzo_encode(char* src_buf, int inlen, char* dst_buf, int& outlen) {
	return lzo1x_1_compress((unsigned char*)src_buf, inlen, (unsigned char*)dst_buf, (lzo_uint*)(&outlen), wrkmem) == LZO_E_OK;
}

bool Compressor::lzo_decode(char* src_buf, int inlen, char* dst_buf, int& outlen) {
	return lzo1x_decompress((unsigned char*)src_buf, inlen, (unsigned char*)dst_buf, (lzo_uint*)(&outlen), NULL) == LZO_E_OK;
}

///////////////////////////////////////////////////////////
//varint compress and decompress
void Compressor::encode_uint(UINT value, char*& buffer) {
	if(value >= (1 << 7)) {
		buffer[0] = (value | 0x80);

		if(value >= (1 << 14)) {
			buffer[1] = (value >> 7) | 0x80;

			if(value >= (1 << 21)) {
				buffer[2] = (value >> 14) | 0x80;

				if(value >= (1<<28)) {
					buffer[3] = (value >> 21) | 0x80;

					buffer[4] = value >> 28;
					buffer += 5;
					return;
				}
				else {
					buffer[3] = value >> 21;
					buffer += 4;
					return;
				}
			}
			else {
				buffer[2] = value >> 14;
				buffer += 3;
				return;
			}
		}
		else {
			buffer[1] = value >> 7;
			buffer += 2;
			return;
		}
	}
	else {
		buffer[0] = value;
		buffer++;
	}
}

void Compressor::decode_uint(char*& buffer, UINT& value) {
	register UINT8 b0 = buffer[0];
	register UINT r0 = (b0 & 0x7f);

	if(!(b0 & 0x80)) {
		value = r0;
		buffer++;
		return;
	}

	register UINT8 b1 = buffer[1];
	register UINT r1 = (b1 & 0x7f) << 7;

	if(!(b1 & 0x80)) {
		value = r1 | r0;
		buffer += 2;
		return;
	}

	register UINT8 b2 = buffer[2];
	register UINT r2 = (b2 & 0x7f) << 14;

	if(!(b2 & 0x80)) {
		value = r2 | r1 | r0;
		buffer += 3;
		return;
	}

	register UINT8 b3 = buffer[3];
	register UINT r3 = (b3 & 0x7f) << 21;

	if(!(b3 & 0x80)) {
		value = r3 | r2 | r1 | r0;
		buffer += 4;
		return;
	}

	value = (buffer[4] << 28) | r3 | r2 | r1 | r0;
	buffer += 5;
}

//把一个负数赋给一个uint，会变成一个很大的整数，所有得先用zigzag编码一下
//貌似现在有某些地方是用uint来write，但是用int来read，或者反过来，
//所以用zigzag来编码会有问题
void Compressor::encode_int(int v, char*& buffer) {
	//利用zigzag编码把int变成uint
	//UINT value =  (v << 1) ^ (v >> 31);
	encode_uint(v, buffer);
}

void Compressor::decode_int(char*& buffer, int& value) {
	UINT v = 0;
	decode_uint(buffer, v);
	value = v;
	//value = (v >> 1) ^ -(int)(v & 1);
}

///////////////////////////////////////////////////////////

void Compressor::print_num(int n) {
	unsigned int n1 = n & 0xFF;
	unsigned int n2 = (n & 0xFF00) >> 8;
	unsigned int n3 = (n & 0xFF0000) >> 16;
	unsigned int n4 = (n & 0xFF000000) >> 24;

	printf("%02X %02X %02X %02X\n", n4, n3, n2, n1);
}

void Compressor::print_buf(char* buf, int size) {
	for(int i=0; i<size; ++i) printf("%02X ", buf[i]);
	puts("");
}
