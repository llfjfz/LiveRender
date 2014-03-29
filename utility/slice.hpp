#ifndef __SLICE__
#define __SLICE__

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <string>

class Slice {
public:
	// Create an empty slice.
	Slice() : data_(""), size_(0) { }

	// Create a slice that refers to d[0,n-1].
	Slice(const char* d, size_t n) : data_(d), size_(n) { }

	// Create a slice that refers to the contents of "s"
	Slice(const std::string& s) : data_(s.data()), size_(s.size()) { }

	const char* data() const { return data_; }
	int size() const { return size_; }
	bool empty() const { return size_ == 0; }
	void clear() { data_ = ""; size_ = 0; }

	char operator[] (int n) const {
		assert(n < size());
		return data_[n];
	}

	void remove_prefix(int n) {
		assert(n <= size());
		data_ += n;
		size_ -= n;
	}

	std::string toString() const { return std::string(data_, size_); }

	// Three-way comparison.  Returns value:
	//   <  0 iff "*this" <  "b",
	//   == 0 iff "*this" == "b",
	//   >  0 iff "*this" >  "b"
	int compare(const Slice& b) const;

	//return true  iff "x" is a prefix of "*this"
	bool starts_with(const Slice& x) const {
		return ( (size_ >= x.size()) && (memcmp(data_, x.data(), x.size()) == 0) );
	}

private:
	const char* data_;
	int size_;
};

inline bool operator==(const Slice& x, const Slice& y) {
	return ((x.size() == y.size()) && (memcmp(x.data(), y.data(), x.size()) == 0));
}

inline bool operator!=(const Slice& x, const Slice& y) {
	return !(x == y);
}

inline int Slice::compare(const Slice& b) const {
	const int min_len = (size_ < b.size()) ? size_ : b.size();
	int r = memcmp(data_, b.data(), min_len);

	if(r == 0) {
		if(size_ < b.size()) r = -1;
		else if(size_ > b.size()) r = 1;
	}
	return r;
}

#endif

