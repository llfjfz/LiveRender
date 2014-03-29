#ifndef __BIT_SET__
#define __BIT_SET__
#include <stdio.h>
#include <cstring>

#define BitSet_Mask 31

class BitSet {
public:
	BitSet(int length);
	void reset(int length);
	void set(int pos);
	bool is_set(int pos);

	void display();

	int count_;
	int length_;
	unsigned int set_[10010];
};

#endif

