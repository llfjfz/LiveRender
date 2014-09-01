#ifndef __LRUCACHE__
#define __LRUCACHE__

#include <windows.h>
#include <stdio.h>
#include <iostream>
using namespace std;
#include "slice.hpp"
#include "utility.h"

struct HashNode {
	int cache_id;
	HashNode* next_hash;
	HashNode* next;
	HashNode* prev;
	int key_length;
	UINT hash;
	char key_data[1];

	Slice key() const {
		return Slice(key_data, key_length);
	}
};

class HashTable {
public:

	HashTable() : length_(0), elems_(0), list_(NULL) { resize(); }
	~HashTable() { delete[] list_; }

	int size() const { return elems_; }

	HashNode* look_up(const Slice& key, UINT hash) {
		return *find_ptr(key, hash);
	}

	HashNode* insert(HashNode* node) {
		HashNode** ptr = find_ptr(node->key(), node->hash);
		HashNode* old = *ptr;
		node->next_hash = (old == NULL ? NULL : old->next_hash);
		*ptr = node;

		if(old == NULL) {
			++elems_;
			if(elems_ > length_) {
				resize();
			}
		}
		return old;
	}

	HashNode* remove(const Slice& key, UINT hash) {
		HashNode** ptr = find_ptr(key, hash);
		HashNode* result = *ptr;
		if(result != NULL) {
			*ptr = result->next_hash;
			--elems_;
		}
		return result;
	}

private:

	// Return a pointer to slot that points to a cache entry that
	// matches key/hash.  If there is no such cache entry, return a
	// pointer to the trailing slot in the corresponding linked list.
	HashNode** find_ptr(const Slice& key, UINT hash) {
		HashNode** ptr = &list_[hash & (length_ - 1)];
		while(*ptr != NULL && ((*ptr)->hash != hash || key != (*ptr)->key()) ) {
			ptr = &(*ptr)->next_hash;
		}
		return ptr;
	}

	void resize() {
		int new_length = 4;
		while(new_length < elems_) {
			new_length <<= 1;
		}
		HashNode** new_list = new HashNode*[new_length];
		memset(new_list, 0, sizeof(new_list[0]) * new_length);
		int count = 0;

		for(int i=0; i<length_; ++i) {
			HashNode* now = list_[i];
			while(now != NULL) {
				HashNode* next = now->next_hash;
				Slice key = now->key();
				int hash = now->hash;
				HashNode** ptr = &new_list[hash & (new_length - 1)];

				now->next_hash = *ptr;
				*ptr = now;
				now = next;
				count++;
			}
		}
		assert(elems_ == count);
		delete[] list_;
		list_ = new_list;
		length_ = new_length;
	}

private:
	int length_;
	int elems_;
	HashNode** list_;
};

class FIFOCache {
public:
	FIFOCache(): last_id(0), capacity_(30000) {
		hit_cnt = 0;
		set_cnt = 0;
		fifo_.next = &fifo_;
		fifo_.prev = &fifo_;
	}

	FIFOCache(int capacity): last_id(0) {
		capacity_ = capacity;
		hit_cnt = 0;
		set_cnt = 0;
		fifo_.next = &fifo_;
		fifo_.prev = &fifo_;
	}

	~FIFOCache() {
		for(HashNode*e=fifo_.next; e!=&fifo_; ) {
			HashNode* next = e->next;
			fifo_release(e);
			e = next;
		}
	}

	static inline UINT hash_slice(const Slice& s) {
		return CacheMgr::hash(s.data(), s.size(), 0);
	}

	void set_capacity(int capacity) { capacity_ = capacity; }
	int get_capacity() const { return capacity_; }

	void fifo_release(HashNode* e) {
		free(e);
	}

	void fifo_remove(HashNode* e) {
		e->next->prev = e->prev;
		e->prev->next = e->next;
	}

	void fifo_append(HashNode* e) {
		e->next = &fifo_;
		e->prev = fifo_.prev;
		e->prev->next = e;
		e->next->prev = e;
	}

	void fifo_replace(HashNode *e, HashNode *old){
		e->prev = old->prev;
		e->next = old->next;
		old->prev->next = e;
		old->next->prev = e;
	}

	

	HashNode* insert(const Slice& key, int& hit_id, int& rep_id) {
		HashNode* e = reinterpret_cast<HashNode*>(malloc(sizeof(HashNode) - 1 + key.size()));
		e->key_length = key.size();
		e->hash = hash_slice(key);
		memcpy(e->key_data, key.data(), key.size());

		HashNode* old = table_.insert(e);

		if(old != NULL) {
			//Cache hit
			e->cache_id = old->cache_id;

			hit_id = e->cache_id;
			rep_id = -1;

			fifo_replace(e, old);
			fifo_release(old);
			//update hit ratio
			hit_cnt++;
		}
		else {
			//Cache miss
			hit_id = -1;
	
			fifo_append(e);
			//如果当前的缓存数量超过capacity，把lru_后面的那个缓存单元淘汰掉
			if(table_.size() > capacity_) {
				HashNode* cursor = fifo_.next;
				fifo_remove(cursor);
				table_.remove(cursor->key(), cursor->hash);

				e->cache_id = cursor->cache_id;
				fifo_release(cursor);
			}
			else {
				e->cache_id = last_id++;  //TODO unlimited increase
			}

			rep_id = e->cache_id;

			//update hit ratio
			set_cnt++;
		}

		return e;
	}

	void erase(const Slice& key, UINT hash) {
		HashNode* e = table_.remove(key, hash);
		if(e != NULL) {
			fifo_remove(e);
			fifo_release(e);
		}
	}
private:
	int capacity_;
	HashNode fifo_;
	HashTable table_;
	int last_id;

	int hit_cnt;
	int set_cnt;

public:
	virtual void get_cache_index(const char* buffer, size_t size, int& hit_id, int& rep_id) {
		insert(Slice(buffer, size), hit_id, rep_id);
	}
};

#endif

