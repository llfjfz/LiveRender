#ifndef __NEW_HASH__
#define __NEW_HASH__

#include <windows.h>
#include <stdio.h>
#include <iostream>
using namespace std;
#include "slice.hpp"

struct YashNode {
	void* val;
	YashNode* next_hash;
	YashNode* next;
	YashNode* prev;
	int key_length;
	UINT hash;
	char key_data[1];

	Slice key() const {
		return Slice(key_data, key_length);
	}
};

class YashTable {
public:

	YashTable() : length_(0), elems_(0), list_(NULL) { resize(); }
	~YashTable() { delete[] list_; }

	int size() const { return elems_; }

	YashNode* look_up(const Slice& key, UINT hash) {
		return *find_ptr(key, hash);
	}

	YashNode* insert(YashNode* node) {
		YashNode** ptr = find_ptr(node->key(), node->hash);
		YashNode* old = *ptr;
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

	YashNode* remove(const Slice& key, UINT hash) {
		YashNode** ptr = find_ptr(key, hash);
		YashNode* result = *ptr;
		if(result != NULL) {
			*ptr = result->next_hash;
			--elems_;
		}
		return result;
	}

	YashNode* look_up(const Slice& key) {
		UINT xxx = hash(key.data(), key.size(), 0);
		return look_up(key, xxx);
	}

	YashNode* insert(const Slice& key, void* val) {
		YashNode* e = reinterpret_cast<YashNode*>(malloc(sizeof(YashNode) - 1 + key.size()));
		e->key_length = key.size();
		e->hash = hash(key.data(), key.size(), 0);
		memcpy(e->key_data, key.data(), key.size());
		e->val = val;

		return insert(e);
	}

private:

	// Return a pointer to slot that points to a cache entry that
	// matches key/hash.  If there is no such cache entry, return a
	// pointer to the trailing slot in the corresponding linked list.
	YashNode** find_ptr(const Slice& key, UINT hash) {
		YashNode** ptr = &list_[hash & (length_ - 1)];
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
		YashNode** new_list = new YashNode*[new_length];
		memset(new_list, 0, sizeof(new_list[0]) * new_length);
		int count = 0;

		for(int i=0; i<length_; ++i) {
			YashNode* now = list_[i];
			while(now != NULL) {
				YashNode* next = now->next_hash;
				Slice key = now->key();
				int hash = now->hash;
				YashNode** ptr = &new_list[hash & (new_length - 1)];

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


	UINT hash(const char* data, size_t n, UINT seed) {
		const UINT m = 0x6a4a793;
		const UINT r = 24;
		const char* limit = data + n;
		UINT h = seed ^ (n * m);

		while(data + 4 < limit) {
			UINT w = *((UINT*)data);
			data += 4;
			h += w;
			h *= m;
			h ^= (h >> 16);
		}

		switch(limit - data) {
		case 3:
			h += data[2] << 16;
			//fall through
		case 2:
			h += data[1] << 8;
			//fall through
		case 1:
			h += data[0];
			h *= m;
			h ^= (h >> r);
			break;
		}
		return h;
	}

private:
	int length_;
	int elems_;
	YashNode** list_;
};

#endif
