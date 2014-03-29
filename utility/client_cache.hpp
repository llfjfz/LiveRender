#ifndef __CLIENT_CACHE__
#define __CLIENT_CACHE__

#include "utility.h"

class ClientCache {
public:

	ClientCache(): hit_cnt(0), set_cnt(0) {}

	void set_cache(int index, const char* cache, int size) {
		memcpy(buffer[index], cache, size);
	}

	char* get_cache(int index) {
		return buffer[index];
	}

private:
	char buffer[30010][205];
	char* cur_ptr;
public:
	int set_cnt, hit_cnt;
};



#endif
