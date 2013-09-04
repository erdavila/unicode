#ifndef TEST_HPP_
#define TEST_HPP_

#include "unicode/unicode.hpp"
#include <initializer_list>
#include <sstream>
#include <string>
#include <vector>
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
