#ifndef __WRAP_DIRECT3DINDEXBUFFER9__
#define __WRAP_DIRECT3DINDEXBUFFER9__

#include "game_server.h"
#include "new_hash.hpp"
#include <map>
#include <iostream>
using namespace std;

class YMesh;

class WrapperDirect3DIndexBuffer9: public IDirect3DIndexBuffer9 {
private:
	int id;
	bool isLock;

public:

	YMesh* get_y_mesh(int vb_id, INT BaseVertexIndex, UINT startIndex);
	void put_y_mesh(int vb_id, INT BaseVertexIndex, UINT startIndex, YMesh* y_mesh);

	//YMesh* y_mesh_;
	//map<int, YMesh*> mesh_map;
	//map<int, YMesh*> :: iterator it;

	YashTable mesh_table_;

	IDirect3DIndexBuffer9* m_ib;
	char* cache_buffer; //析构的时候记得要删掉
	char * ram_buffer; // the temp buffer for lock
	bool isFirst;
	bool SetFirst;//set isFirst true when true
	bool changed;
	DWORD Usage;
	D3DFORMAT Format;
	D3DPOOL Pool;

	int length;
	int PrepareIndexBuffer();
	BufferLockData m_LockData;
	bool isSent;// the index buffer sent or not
	static HashSet m_list;
	static int ins_count;
	WrapperDirect3DIndexBuffer9(IDirect3DIndexBuffer9* ptr, int _id, int _length);
	WrapperDirect3DIndexBuffer9(IDirect3DIndexBuffer9* ptr, int _id);
	LPDIRECT3DINDEXBUFFER9 GetIB9();
	void SetID(int id);
	int GetID();
	int GetLength();

	void read_data_from_buffer(char** ptr, int offest, int size);
	void write_data_to_buffer(char* ptr, int offest, int size);

	static WrapperDirect3DIndexBuffer9* GetWrapperIndexedBuffer9(IDirect3DIndexBuffer9* base_indexed_buffer);

public:
	/*** IUnknown methods ***/
	COM_METHOD(HRESULT, QueryInterface)(THIS_ REFIID riid, void** ppvObj);
	COM_METHOD(ULONG,AddRef)(THIS);
	COM_METHOD(ULONG,Release)(THIS);

	/*** IDirect3DResource9 methods ***/
	COM_METHOD(HRESULT, GetDevice)(THIS_ IDirect3DDevice9** ppDevice);
	COM_METHOD(HRESULT, SetPrivateData)(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags);
	COM_METHOD(HRESULT, GetPrivateData)(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData);
	COM_METHOD(HRESULT, FreePrivateData)(THIS_ REFGUID refguid);
	COM_METHOD(DWORD, SetPriority)(THIS_ DWORD PriorityNew);
	COM_METHOD(DWORD, GetPriority)(THIS);
	COM_METHOD(void, PreLoad)(THIS);
	COM_METHOD(D3DRESOURCETYPE, GetType)(THIS);
	COM_METHOD(HRESULT, Lock)(THIS_ UINT OffsetToLock,UINT SizeToLock,void** ppbData,DWORD Flags);
	COM_METHOD(HRESULT, Unlock)(THIS);
	COM_METHOD(HRESULT, GetDesc)(THIS_ D3DINDEXBUFFER_DESC *pDesc);
};

#endif
