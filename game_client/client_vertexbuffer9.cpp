#include "client_vertexbuffer9.h"
#include <assert.h>
#include <stdlib.h>
#include "float_helper.h"

#define INT_COMPRESS
//#define FLOAT_COMPRESS
#define COMPRESS_TO_DWORD
// #define COMPRESS_TO_BYTE

float mx, my, mz;

ClientVertexBuffer9::ClientVertexBuffer9(IDirect3DVertexBuffer9* ptr, int _length): m_vb(ptr), length(_length), isFirst(true) {
	Log::log("clientVertexBuffer constructor entered, len:%d\n", _length);
	//this->vb_data = (char *)malloc(sizeof(char ) * _length);
	if(!this->vb_data){
		Log::log("clientVertexBuffer constructor malloc vb_data failed!\n");
	}

	vertex_count_ = 0;

	remain_vertices_.clear();
}

HRESULT ClientVertexBuffer9::Lock(UINT OffestToLock, UINT SizeToLock, DWORD Flags) {
	m_LockData.OffsetToLock = OffestToLock;
	m_LockData.SizeToLock = SizeToLock;
	m_LockData.Flags = Flags;
	if(SizeToLock == 0) m_LockData.SizeToLock = length - OffestToLock;

	return S_OK;
}

HRESULT ClientVertexBuffer9::Unlock() {
	/*
	m_LockData.OffsetToLock = cc.read_uint();
	m_LockData.SizeToLock = cc.read_uint();
	m_LockData.Flags = cc.read_uint();
	*/

	//Log::log("ClientVertexBuffer9::Unlock(), OffestToLock=%d, SizeToLock=%d Bytes, Flags=%d\n", m_LockData.OffsetToLock, m_LockData.SizeToLock, m_LockData.Flags);

	UpdateVertexBuffer();

	return D3D_OK;
}

HRESULT ClientVertexBuffer9::SetDecimateResult() {
	Log::log("ClientVertexBuffer9::SetDecimateResult()\n");

	remain_vertices_.clear();
	int sz = cc.read_int();
	
	for(int i=0; i<sz; ++i) {
		int p = cc.read_int();
		remain_vertices_.push_back(p);
	}
	
	vertex_count_ = sz;
	Log::log("ClientVertexBuffer9::SetDecimateResult(), recv sz=%d\n", sz);

	return D3D_OK;
}

HRESULT ClientVertexBuffer9::SetVertexBufferFormat() {
	Log::log("ClientVertexBuffer9::SetVertexBufferFormat() called\n");
	vb_format_.clear();
	int sz = cc.read_uint();

	for(int i=0; i<sz; ++i) {
		int flag = cc.read_char();
		int offest = cc.read_uchar();
		int size = cc.read_uchar();

		Log::log("flag=%d, offest=%d, size=%d\n", flag, offest, size);

		switch(flag) {
		case 1:
			vb_format_.push_back(decl_node(flag, offest, size, decode_position));
			break;
		case 2:
			vb_format_.push_back(decl_node(flag, offest, size, decode_normal));
			break;
		case 4:
			vb_format_.push_back(decl_node(flag, offest, size, decode_tex));
			break;
		case 8:
			vb_format_.push_back(decl_node(flag, offest, size, decode_tangent));
			break;
		case 16:
			vb_format_.push_back(decl_node(flag, offest, size, decode_color));
			break;
		case 32:
			vb_format_.push_back(decl_node(flag, offest, size, decode_weight));
			break;
		case 64:
			vb_format_.push_back(decl_node(flag, offest, size, decode_indice));
			break;
		default:
			break;
		}
	}

	return D3D_OK;
}

IDirect3DVertexBuffer9* ClientVertexBuffer9::GetVB() {
	return m_vb;
}

int ClientVertexBuffer9::GetLength() {
	return length;
}

float maxX = 0.0, maxY = 0.0, maxZ=  0.0;
void ClientVertexBuffer9::UpdateVertexBuffer() {
	Log::log("ClientVertexBuffer9::UpdateVertexBuffer() called\n");

	char* vb_ptr;
	if(isFirst) {
		m_vb->Lock(0, 0, (void**)&vb_ptr, m_LockData.Flags);
		memset(vb_ptr, 0, length);
		m_vb->Unlock();
		isFirst = false;
	}

	//int mode = cc.read_int();
	int stride = cc.read_int();

	//HRESULT hr = m_vb->Lock(m_LockData.OffsetToLock, m_LockData.SizeToLock, (void**)&vb_ptr, D3DLOCK_NOOVERWRITE);//m_LockData.Flags);
	HRESULT hr = m_vb->Lock(0, 0, (void**)&vb_ptr, D3DLOCK_NOOVERWRITE);//m_LockData.Flags);

	if(SUCCEEDED(hr)) {
		Log::log("lock success, ptr=%d\n", vb_ptr);
	}
	else {
		Log::log("lock failed\n");
	}
	
	/*
	if(remain_vertices_.size() == 0) {
		for(int i=0; i<length/stride; ++i) remain_vertices_.push_back(i);
		Log::log("up size=%d\n", remain_vertices_.size());
	}
	*/

	//Log::log("server count=%d\n", server_count);
	//Log::log("server remain size=%d\n", server_remain);
	Log::log("client remain size=%d\n", remain_vertices_.size());

	int bs_count = cc.read_int();

#ifdef USE_FLOAT_COMPRESS
	mx = cc.read_float();
	my = cc.read_float();
	mz = cc.read_float();
#endif

	
	for(int i=0; i<bs_count; ++i) {
	//for(int i=0; i<remain_vertices_.size(); ++i) {
		int x = cc.read_ushort();
		int idx = x * stride;
		//int idx = remain_vertices_[i] * stride;
			
		//cc.read_byte_arr(vb_ptr + idx, stride);
			
			
		for(int j=0; j<vb_format_.size(); ++j) {
			decl_node& nd = vb_format_[j];
			nd.func_(vb_ptr + idx, nd.offest_, nd.size_);
		}
			
	}
	

	m_vb->Unlock();
}

void decode_position(char* pos, int offest, int size) {
#ifndef USE_FLOAT_COMPRESS
	
	float x = cc.read_float();
	float y = cc.read_float();
	float z = cc.read_float();
	
	
	*( (float*)(pos + offest) ) = x;
	*( (float*)(pos + offest + 4) ) = y;
	*( (float*)(pos + offest + 8) ) = z;
	
	
#else

	DWORD val = cc.read_uint();
	decompress_pos_to_buffer(val, (float*)(pos + offest), mx, my, mz);
#endif

	//Log::slog("position: x=%f, y=%f, z=%f\n", x, y, z);
}

void decode_normal(char* pos, int offest, int size) {
#ifndef USE_FLOAT_COMPRESS
	float nx = cc.read_float();
	float ny = cc.read_float();
	float nz = cc.read_float();

	*( (float*)(pos + offest) ) = nx;
	*( (float*)(pos + offest + 4) ) = ny;
	*( (float*)(pos + offest + 8) ) = nz;
#else

	DWORD val = cc.read_uint();
	decompress_normal_to_buffer(val, (float*)(pos + offest));
#endif

	//Log::slog("normal: nx=%f, ny=%f, nz=%f\n", nx, ny, nz);
}

void decode_tex(char* pos, int offest, int size) {
	cc.read_vec((float*)(pos + offest), size);
}

void decode_tangent(char* pos, int offest, int size) {
#ifndef USE_FLOAT_COMPRESS
	float tx = cc.read_float();
	float ty = cc.read_float();
	float tz = cc.read_float();
	float ta = cc.read_float();

	*( (float*)(pos + offest) ) = tx;
	*( (float*)(pos + offest + 4) ) = ty;
	*( (float*)(pos + offest + 8) ) = tz;
	*( (float*)(pos + offest + 12) ) = ta;
#else

	DWORD val = cc.read_uint();
	decompress_tangent_to_buffer(val, (float*)(pos + offest));
#endif
}

void decode_color(char* pos, int offest, int size) {
	cc.read_byte_arr(pos + offest, size);
}

void decode_weight(char* pos, int offest, int size) {

	float tx = cc.read_float();
	float ty = cc.read_float();
	float tz = cc.read_float();
	float ta = cc.read_float();

	*( (float*)(pos + offest) ) = tx;
	*( (float*)(pos + offest + 4) ) = ty;
	*( (float*)(pos + offest + 8) ) = tz;
	*( (float*)(pos + offest + 12) ) = ta;
}

void decode_indice(char* pos, int offest, int size) {
	unsigned char tx = cc.read_uchar();
	unsigned char ty = cc.read_uchar();
	unsigned char tz = cc.read_uchar();
	unsigned char ta = cc.read_uchar();

	*( (unsigned char*)(pos + offest) ) = tx;
	*( (unsigned char*)(pos + offest + 1) ) = ty;
	*( (unsigned char*)(pos + offest + 2) ) = tz;
	*( (unsigned char*)(pos + offest + 3) ) = ta;
}
