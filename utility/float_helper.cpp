#include "float_helper.h"
#include "utility.h"

DWORD compress_pos_to_dword(float x, float y, float z, float mx, float my, float mz) {
	DWORD ret = 0;

	DWORD t1 = (unsigned short)( (x / mx * 0.5f + 0.5f) * 1023.0f );
	DWORD t2 = (unsigned short)( (y / my * 0.5f + 0.5f) * 1023.0f );
	DWORD t3 = (unsigned short)( (z / mz * 0.5f + 0.5f) * 1023.0f );

	ret = ( (t1<<22) & 0xffc00000 ) | ( (t2<<12) & 0x003ff000 ) | ( t3 & 0x00000fff );
	return ret;
}

DWORD compress_tangent_to_dword(float tx, float ty, float tz, float ta) {
	DWORD ret = 0;
	DWORD t1 = (unsigned long)((tx * 0.5f + 0.5f) *1023.0);
	DWORD t2 = (unsigned long)((ty * 0.5f + 0.5f) *1023.0);
	DWORD t3 = (unsigned long)((tz * 0.5f + 0.5f) *1023.0);

	ret = ( (t1<<22) & 0xffc00000 ) | ( (t2<<12) & 0x003ff000 ) | ( (t3<<2) & 0x00000fff );

	if(ta < 0) {
		ret |= 0x00000002;
	}
	else {
		ret &= 0xfffffffd;
	}

	if(abs(abs(ta) -1) - 1 > eps){
		ret |= 1;
	}
	else{
		ret &= 0xfffffffe;
	}
	return ret;
}

DWORD compress_normal_to_dword(float nx, float ny, float nz) {
	DWORD ret = 0;

	DWORD t1 = (unsigned short)( (nx * 0.5f + 0.5f) * 1023.0f );
	DWORD t2 = (unsigned short)( (ny * 0.5f + 0.5f) * 1023.0f );
	DWORD t3 = (unsigned short)( (nz * 0.5f + 0.5f) * 1023.0f );

	ret = ( (t1<<22) & 0xffc00000 ) | ( (t2<<12) & 0x003ff000 ) | ( t3 & 0x00000fff );
	return ret;
}

BYTE compress_float_to_byte(float f) {
	return (unsigned char)((f * 0.5f + 0.5f) * 255);
}

USHORT compress_float_to_short(float f) {
	return (unsigned short)((f * 0.5f + 0.5f) * 65535);
}

void decompress_pos_to_buffer(DWORD val, float* buffer, float mx, float my, float mz) {
	unsigned short num;

	num = (unsigned short)((val&0xffc00000)>>22);
	*buffer = ( num * 2.0f / 1023.0f - 1.0f ) * mx;
	buffer++;

	num = (unsigned short)((val&0x003ff000)>>12);
	*buffer = ( num * 2.0f / 1023.0f - 1.0f ) * my;
	buffer++;

	num = (unsigned short)((val&0x00000fff));
	*buffer = ( num * 2.0f / 1023.0f - 1.0f ) * mz;
}

void decompress_tangent_to_buffer(DWORD val, float* buffer) {
	short num;

	num = (short)((val&0xffc00000)>>22);
	*buffer = (num) * 2.0f / 1023.0f - 1.0f;
	buffer++;
	
	num = (short)((val&0x003ff000)>>12);
	*buffer = (num) * 2.0f / 1023.0f - 1.0f;
	buffer++;

	num = (short)((val&0x00000ffc)>>2);
	*buffer = (num) * 2.0f / 1023.0f - 1.0f;
	buffer++;

	num = (short)((val&0x00000003));
	float a = 0.0;
	if(num|0x00000001){
		a  = 1.0;
	}
	else{
		a = 0.0;
	}
	if(num|0x00000002){
		a = -a;
	}else{

	}
	*buffer = a;
}

void decompress_normal_to_buffer(DWORD val, float* buffer) {
	unsigned short num;

	num = (unsigned short)((val&0xffc00000)>>22);
	*buffer = num * 2.0f / 1023.0f - 1.0f;
	buffer++;

	num = (unsigned short)((val&0x003ff000)>>12);
	*buffer = num * 2.0f / 1023.0f - 1.0f;
	buffer++;

	num = (unsigned short)((val&0x00000fff));
	*buffer = num * 2.0f / 1023.0f - 1.0f;
}

void decompress_byte_to_buffer(BYTE val, float* buffer) {
	*buffer = val * 2.0f / 255.0f - 1.0f;
}

void decompress_short_to_buffer(USHORT val, float* buffer) {
	*buffer = val * 2.0f / 65535.0f - 1.0f;
}
