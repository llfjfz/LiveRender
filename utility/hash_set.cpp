#include "utility.h"

HashSet::HashSet() {
	for(int i=0; i<HASHSETSIZE; ++i) {
		m_pHead[i] = NULL;
	}
}

PVOID HashSet::GetDataPtr(int id) {
	HashLinkedList* pThis = m_pHead[GetHash(id)];
	while(pThis) {
		if(pThis->id == id) {
			return pThis->pData;
		}
		pThis = pThis->pNext;
	}
	return NULL;
}

PVOID HashSet::GetDataPtr(PVOID pKey) {
	HashLinkedList* pThis = m_pHead[GetHash(pKey)];
	while(pThis)
	{
		if(pThis->pKey == pKey)
		{
			return pThis->pData;
		}
		pThis = pThis->pNext;
	}
	return NULL;
}

bool HashSet::AddMember(int id, PVOID pData) {
	UINT hash = GetHash(id);
	HashLinkedList* pThis = new HashLinkedList;
	if(pThis == NULL) {
		return false;
	}
	pThis->pData = pData;
	pThis->id = id;
	pThis->pNext = m_pHead[hash];
	m_pHead[hash] = pThis;

	return true;
}

bool HashSet::AddMember(PVOID pKey, PVOID pData)
{
	UINT Hash = GetHash(pKey);
	HashLinkedList* pThis = new HashLinkedList;
	if(pThis == NULL)
	{
		return false;
	}

	pThis->pNext = m_pHead[Hash];
	pThis->pKey = pKey;
	pThis->pData = pData;
	m_pHead[Hash] = pThis;
	return true;
}

bool HashSet::DeleteMember(int id) {
	UINT hash = GetHash(id);
	HashLinkedList* pThis = m_pHead[hash];
	HashLinkedList* pLast = 0L;

	if(m_pHead[hash]->id == id) {
		m_pHead[hash] = pThis->pNext;
		delete pThis;
		return true;
	}
	else {
		pLast = pThis;
		pThis = pThis->pNext;
	}

	while(pThis) {
		if(pThis->id == id) {
			pLast->pNext = pThis->pNext;
			delete pThis;
			return true;
		}
		pLast = pThis;
		pThis = pThis->pNext;
	}
	return false;
}


bool HashSet::DeleteMember(PVOID pKey)
{
	UINT Hash = GetHash(pKey);
	HashLinkedList* pThis = m_pHead[Hash];
	HashLinkedList* pLast = 0L;

	if( m_pHead[Hash]->pKey == pKey )
	{
		m_pHead[Hash] = pThis->pNext;
		delete pThis;
		return true;
	}
	else
	{
		pLast = pThis;
		pThis = pThis->pNext;
	}

	while( pThis )
	{
		if( pThis->pKey == pKey )
		{
			pLast->pNext = pThis->pNext;
			delete pThis;
			return true;
		}
		pLast = pThis;
		pThis = pThis->pNext;
	}
	return false;
}


