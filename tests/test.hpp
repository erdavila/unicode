#ifndef TEST_HPP_
#define TEST_HPP_

#include "unicode/unicode.hpp"
#include <initializer_list>
#include <sstream>
#include <string>
#include <vector>
#ifdef __CYGWIN__
// Why can't Google Test see the regular declarations of the functions below on Cygwin GCC?!
# include <cstdio>
# include <cstring>
int __attribute__((__cdecl__)) fileno (FILE *);
char *strdup(const char *s);
FILE *fdopen(int fd, const char *mode);
#endif
#include "gtest/gtest.h"


struct bytes : std::vector<unicode::byte> {
	bytes(std::initializer_list<char> lst) : std::vector<unicode::byte>(lst.begin(), lst.end()) {}
};

inline std::string to_hex(unsigned long value, size_t width) {
	std::ostringstream ostrs;
	ostrs.width(width);
	ostrs.fill('0');
	ostrs << std::hex << std::uppercase << value;
	return ostrs.str();
}


#endif /* TEST_HPP_ */
