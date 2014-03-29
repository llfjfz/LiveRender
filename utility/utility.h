#ifndef __UTILITY__
#define __UTILITY__

#include <WinSock2.h>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <algorithm>
#include <string>
#include <windows.h>
#include <stack>
#include "opcode.h"

using namespace std;

//Begin Compile Condition

//#define MEM_FILE_TEX
//#define SAVE_IMG
#define SERVER_INPUT
//#define USE_VARINT
//#define LOCAL_IMG
//#define USE_FLOAT_COMPRESS
#define USE_CACHE

//#define USE_CLIENT_INPUT

//#define ENABLE_LOG
#define ENABLE_NETWORK_COMPRESS
#define ENABLE_SERVER_RENDERING

//#define Enable_Command_Validate
//End Compile Condition

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma warning(disable : 4996)

#define eps (1e-4)

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define MAX_CHAR 1024
//#define MAXPACKETSIZE 7500000
#define MAXPACKETSIZE 20000000
#define MAX_FPS 20
#define BUFFER_BLOCK_SIZE 1
#define BUFFER_BLOCK_LEVEL 0

#define Cache_Length_Limit 100

#define Max_Func_Count 200
#define Max_Buf_Size 2600

#define Source_Count 4

#define Cache_Use	0
#define Cache_Set	1


#define HASHSETSIZE 128
#define HASHSETMASK	127

#define CACHE_MODE_INIT 0
#define CACHE_MODE_COPY 1
#define CACHE_MODE_DIFF 2
#define CACHE_MODE_COM	3
#define CACHE_MODE_DIFF_AND_COM 4
#define CACHE_MODE_PRESS_NORMAL_TANGENT 5

//Object Type
#define DEVICE_OBJ				0
#define INDEX_BUFFER_OBJ		1
#define VERTEX_BUFFER_OBJ		2
#define PIXEL_SHADER_OBJ		3
#define	VERTEX_SHADER_OBJ		4
#define TEXTURE_OBJ				5
#define STATE_BLOCK_OBJ			6
#define VERTEX_DECL_OBJ			7

#define SAFE_RELEASE(obj) \
	if(obj) { delete obj; obj = NULL; }

struct BufferLockData
{
	UINT OffsetToLock;
	UINT SizeToLock;
	DWORD Flags;
	HANDLE Handle;
	VOID* pRAMBuffer;
	//VOID* pVideoBuffer;
	bool Create;
};

namespace CacheMgr {
	UINT hash(const char* data, size_t n, UINT seed);
}

#include "log.h"
#include "hash_set.h"

#include "compressor.h"

#include "config.h"

#include "buffer.h"

#include "network.h"
#include "command_server.h"
#include "command_client.h"

#endif