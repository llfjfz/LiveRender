#include "utility.h"

namespace CacheMgr {

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

}