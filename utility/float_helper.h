#ifndef __FLOAT_HELPER__
#define __FLOAT_HELPER__
#include <stdio.h>
#include <cstring>

typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef unsigned short USHORT;

DWORD compress_pos_to_dword(float x, float y, float z, float mx, float my, float mz);
DWORD compress_tangent_to_dword(float tx, float ty, float tz, float ta);
DWORD compress_normal_to_dword(float nx, float ny, float nz);
BYTE compress_float_to_byte(float f);
USHORT compress_float_to_short(float f);

void decompress_pos_to_buffer(DWORD val, float* buffer, float mx, float my, float mz);
void decompress_tangent_to_buffer(DWORD val, float* buffer);
void decompress_normal_to_buffer(DWORD val, float* buffer);
void decompress_byte_to_buffer(BYTE val, float* buffer);
void decompress_short_to_buffer(USHORT val, float* buffer);

#endif

