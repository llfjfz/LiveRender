#ifndef __HASH_SET__
#define __HASH_SET__

#include "utility.h"

struct HashLinkedList {
	int id;
	PVOID pKey;
	PVOID pData;
	HashLinkedList* pNext;
};

class HashSet {
public:
	HashSet();
	PVOID GetDataPtr(int id);
	PVOID GetDataPtr(PVOID pKey);
	bool AddMember(int id, PVOID pData);
	bool AddMember(PVOID pKey, PVOID pData);
	bool DeleteMember(PVOID pKey);
	bool DeleteMember(int id);
	__forceinline UINT GetHash(PVOID pKey) {
		DWORD Key = (DWORD)pKey;
		return (( Key >> 3 ^ Key >> 7 ^ Key >> 11 ^ Key >> 17 ) & HASHSETMASK);
	}
	__forceinline UINT GetHash(int id) {
		return (( id >> 3 ^ id >> 7 ^ id >> 11 ^ id >> 17 ) & HASHSETMASK);
	}
private:
	HashLinkedList* m_pHead[HASHSETSIZE];
};

#endif