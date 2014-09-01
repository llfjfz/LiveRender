#ifndef __LFUCACHE__
#define __LFUCACHE__

#include <windows.h>
#include <stdio.h>
#include <iostream>
using namespace std;
#include "slice.hpp"
#include "utility.h"
#include <list>
#include <algorithm>

struct FreqNode;

struct HashNode {
	int cache_id;
	HashNode* next_hash;
	HashNode* next;
	HashNode* prev;
	FreqNode* freqNode;
	int key_length;
	UINT hash;
	char key_data[1];

	Slice key() const {
		return Slice(key_data, key_length);
	}
};

struct FreqNode {
	int freq;
	FreqNode* fn_prev;
	FreqNode* fn_next;
	HashNode* hn_prev;
	HashNode* hn_next;

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
			new_length *= 2;
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

class LFUCache {
public:
	LFUCache(int capacity): last_id(0), capacity_(capacity) {
		hit_cnt = 0;
		set_cnt = 0;
		head_.fn_prev = head_.fn_next = &head_;
		head_.hn_prev = head_.hn_next = NULL;
	}

	~LFUCache() {
		for(FreqNode* f=head_.fn_next; f!=&head_;){
			HashNode* e = f->hn_prev;
			if(e!=NULL){
				while(e->next != e){
					HashNode* next = e->next;
					lfu_release(e);
					e = next;
				}
				lfu_release(e);
			}
			FreqNode* next = f->fn_next;
			lfu_release(f);
			f = next;
		}
		lfu_release(&head_);
	}

	static inline UINT hash_slice(const Slice& s) {
		return CacheMgr::hash(s.data(), s.size(), 0);
	} 

	void set_capacity(int capacity) { capacity_ = capacity; }
	int get_capacity() const { return capacity_; }

	void lfu_release(void* node){
		free(node);
	}

	void hashnode_remove(FreqNode* f, HashNode* e) {
		if(e->prev == e && e->next == e){
			//Only one HashNode in the frequency list
			f->hn_prev = f->hn_next = NULL;
		}
		else if(e->prev == e){
			//The first HashNode
			f->hn_prev = e->next;
			e->next->prev = e->next;
		}
		else if(e->next == e){
			//The last HashNode
			f->hn_next = e->prev;
			e->prev->next = e->prev;
		}
		else{
			//The middle HashNode
			e->next->prev = e->prev;
			e->prev->next = e->next;
		}		
	}

	void hashnode_append(FreqNode* f, HashNode* e){
		//Appending e after the FreqNode, at head of HashNode with same frenqucy
		if(f->hn_prev == NULL || f->hn_next == NULL){
			//The freqNode is newly added
			f->hn_prev = f->hn_next = e->prev = e->next = e;
		}
		else{
			e->next = f->hn_prev;
			f->hn_prev->prev = e;
			e->prev = e;
			f->hn_prev = e;
		}	
	}

	void freqnode_append(FreqNode* cur, FreqNode* f){
		//head_.fn_next indicates the first node, head_.fn_prev indicates the last node
		f->fn_prev = cur;
		f->fn_next = cur->fn_next;
		cur->fn_next->fn_prev = f;
		cur->fn_next = f;
	}
	
	void freqnode_remove(FreqNode* f){
		f->fn_next->fn_prev = f->fn_prev;
		f->fn_prev->fn_next = f->fn_next;
	}

	HashNode* insert(const Slice& key, int& hit_id, int& rep_id) {
		HashNode* e = reinterpret_cast<HashNode*>(malloc(sizeof(HashNode) - 1 + key.size()));
		e->key_length = key.size();
		e->hash = hash_slice(key);
		memcpy(e->key_data, key.data(), key.size());

		HashNode* old = table_.insert(e);
		int nextFreq;
		FreqNode* currentNode;
		FreqNode* nextFreqNode;
		int flag = 0;

		if(old != NULL){
			Log::slog("LFUCache cache hit.\n");
			//Cache hit
			e->cache_id = old->cache_id;

			hit_id = e->cache_id;
			rep_id = -1;
			
			currentNode = old->freqNode;
			nextFreqNode = currentNode->fn_next;
			nextFreq = currentNode->freq + 1;

			if(nextFreqNode == NULL || nextFreq != nextFreqNode->freq){
				//Create a new FreqNode
				FreqNode* newNode = reinterpret_cast<FreqNode*>(malloc(sizeof(FreqNode)));
				newNode->freq = nextFreq;
				newNode->fn_prev = NULL;
				newNode->fn_next = NULL;
				newNode->hn_prev = NULL;
				newNode->hn_next = NULL;
				nextFreqNode = newNode;
				flag = 1;
				Log::slog("LFUCache newNode, freq=%d, prev=%p, next=%p\n", newNode->freq, newNode->hn_prev, newNode->hn_next);
			}		

			if(1 == flag){
				freqnode_append(currentNode, nextFreqNode);
			}
			hashnode_append(nextFreqNode, e);

			Log::slog("LFUCache nextFreqNode, freq=%d, prev=%p, next=%p, address=%p\n", nextFreqNode->freq, nextFreqNode->hn_prev, nextFreqNode->hn_next, nextFreqNode);
					
			e->freqNode = nextFreqNode;

			Log::slog("LFUCache HashNode, freqNode=%p, prev=%p, next=%p, address=%p\n", e->freqNode, e->prev, e->next, e);

			hashnode_remove(currentNode, old);
			lfu_release(old);		

			hit_cnt++;

		}
		else {
			Log::slog("LFUCache cache missing.\n");
			//Cache miss
			hit_id = -1;

			nextFreq = 1;
			nextFreqNode = head_.fn_next;
			if(nextFreqNode == NULL || nextFreqNode->freq != 1){
				FreqNode* newNode = reinterpret_cast<FreqNode*>(malloc(sizeof(FreqNode)));
				newNode->freq = 1;
				newNode->fn_prev = NULL;
				newNode->fn_next = NULL;
				newNode->hn_prev = NULL;
				newNode->hn_next = NULL;
				nextFreqNode = newNode;
				flag = 1;
				Log::slog("LFUCache newNode, freq=%d, prev=%p, next=%p\n", newNode->freq, newNode->hn_prev, newNode->hn_next);
			}

			if(1 == flag){
				//New FreqNode adding
				freqnode_append(&head_, nextFreqNode);
			}

			hashnode_append(nextFreqNode, e);

			Log::slog("LFUCache nextFreqNode, freq=%d, prev=%p, next=%p, address=%p\n", nextFreqNode->freq, nextFreqNode->hn_prev, nextFreqNode->hn_next, nextFreqNode);

			e->freqNode = nextFreqNode;

			Log::slog("LFUCache HashNode, freqNode=%p, prev=%p, next=%p, address=%p\n", e->freqNode, e->prev, e->next, e);

			//Cache overflow
			if(table_.size() > capacity_){
				Log::slog("LFUCache cache overflow.\n");
				//Only consider the frequency == 1 HashNode
				HashNode* cursor = nextFreqNode->hn_next;
				if(cursor == NULL)
					Log::slog("Replace node is NULL\n");
				hashnode_remove(nextFreqNode, cursor);
				table_.remove(cursor->key(), cursor->hash);

				e->cache_id = cursor->cache_id;
				lfu_release(cursor);
			}
			else {
				e->cache_id = last_id++;  //last_id <= capacity
			}

			rep_id = e->cache_id;

			//update set ratio
			set_cnt++;
		}
		Log::slog("LFUCache hit_id=%d, rep_id=%d, last_id=%d\n", hit_id, rep_id, last_id);
		
		return e;
		
	}

	void erase(const Slice& key, UINT hash) {
		HashNode* e = table_.remove(key, hash);
		if(e != NULL) {
			hashnode_remove(e->freqNode, e);
			lfu_release(e);
		}
	}

	virtual void get_cache_index(const char* buffer, size_t size, int& hit_id, int& rep_id) {
		insert(Slice(buffer, size), hit_id, rep_id);
	}


private:
	int capacity_;
	int last_id;
	HashTable table_;
	FreqNode head_; 
	int hit_cnt;
	int set_cnt;

};

#endif