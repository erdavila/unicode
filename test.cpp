#include "unicode.hpp"
#include <sstream>
#include "gtest/gtest.h"
using namespace std;
using namespace unicode;

TEST(GoogleTest2, Fatal) {
	EXPECT_TRUE(false);
	EXPECT_FALSE(true);
}

TEST(GoogleTest2, NonFatal) {
	ASSERT_TRUE(false);
	ASSERT_FALSE(true);
}

/*
int main() {
	utf8::Encoder encoder;
	utf8::Decoder decoder;

	istringstream istrs;
	auto is = createInputStream<utf8, utf32>(istrs);

	ostringstream ostrs;
	auto os = createOutputStream<utf32, utf8>(ostrs);
}
*/
