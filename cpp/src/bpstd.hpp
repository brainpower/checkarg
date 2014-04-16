#ifndef BPSTD_HXX
#define BPSTD_HXX

#include <string>
#include <deque>
#include <sstream>

#include <cstdio>
#include <cstdarg>
#include <alloca.h>

namespace bpstd{
	std::string stringf(const char* format, ...);
	void string_getlines(std::deque<std::string> &v, const std::string &str);
	std::deque<std::string> string_getlines(const std::string &str);
	void string_split(std::deque<std::string> &v, const char c, const std::string &str);
	std::deque<std::string> string_split(const char c, const std::string &str);
	void string_join(std::string &res, const char c, const std::deque<std::string> &v);
	void string_join(std::string &res, const std::deque<std::string> &v);
	std::string string_join(const char c, const std::deque<std::string> &v);
	std::string string_join(const std::deque<std::string> &v);
	std::string string_join(const std::deque<std::string> &v, uint s);
	std::string string_join(const std::deque<std::string> &v, uint s, uint e);
	void strip_line_comments(std::string &s, const char c, const std::string &raw);
	std::string strip_line_comments(const char c, const std::string &raw);


	// checks, if a vector contains the given item; O(n)
	template<typename T>
	bool deq_contains(std::deque< T > v, T i){
		for(auto &item : v)
			if(item == i)
				return true;
		return false;
	}

	// returns the index of given item in an array,
	// if nothing can be found, -1 is returned; O(n)
	template < typename T >
	int indexOf(T array[], size_t size, const T c){
		for(int i=0;i<size;++i){
			if(array[i] == c)
				return i;
		}
		return -1;
	}
}
#endif // BPSTD_HXX
