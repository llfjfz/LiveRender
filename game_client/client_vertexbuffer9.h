#ifndef __CLIENT_VERTEXBUFFER__
#define __CLIENT_VERTEXBUFFER__

#include "game_client.h"

void decode_position(char* pos, int offest, int size);
void decode_normal(char* pos, int offest, int size);
void decode_tex(char* pos, int offest, int size);
void decode_tangent(char* pos, int offest, int size);
void decode_color(char* pos, int offest, int size);
void decode_weight(char* pos, int offest, int size);
void decode_indice(char* pos, int offest, int size);

typedef void (*decode_func)(char* pos, int offest, int size);

struct decl_node {
	int flag_;
	int offest_;
	int size_;
	decode_func func_;

	decl_node(int flag, int offest, int size, decode_func func): flag_(flag), offest_(offest), size_(size), func_(func) {

	}
};

class ClientVertexBuffer9 {
private:
	IDirect3DVertexBuffer9* m_vb;

	vector<decl_node> vb_format_;
	
	int length;
	bool isFirst;
	char * vb_data;

	
	short streamNumber, offsetInBytes;
	UCHAR cp_off, ct_off, ctt_of, cn_off, ctt_size;
	float cMaxX, cMaxY, cMaxZ; // bound box

	vector<int> remain_vertices_;

public:

	int stride;
	int vertex_count_;

	void UpdateVertexBuffer();
	BufferLockData m_LockData;
	ClientVertexBuffer9(IDirect3DVertexBuffer9* ptr, int _length);
	HRESULT Lock(UINT OffestToLock, UINT SizeToLock, DWORD Flags);
	HRESULT Unlock();

	HRESULT SetVertexBufferFormat();
	HRESULT SetDecimateResult();

	IDirect3DVertexBuffer9* GetVB();
	int GetLength();
};

#endif
