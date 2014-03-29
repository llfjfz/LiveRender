#include "bit_set.h"

BitSet::BitSet(int length): length_(length) {
	memset(set_, 0, sizeof set_);
	count_ = 0;
}

void BitSet::reset(int length) {
	count_ = 0;
	length_ = length;
	memset(set_, 0, sizeof set_);
}

void BitSet::set(int pos) {
	count_++;
	set_[pos >> 5] |= ( 1u << (pos & BitSet_Mask) );
}

bool BitSet::is_set(int pos) {
	unsigned int st = set_[pos >> 5];
	return ( st & ( 1u << (pos & BitSet_Mask) ) ) > 0;
}

void BitSet::display() {
	for(int i=0; i<length_; ++i) {
		if(is_set(i)) printf("%d is set\n", i);
	}
}


