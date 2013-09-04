#include "unicode/unicode.hpp"
#include "gtest/gtest.h"
using namespace unicode;
using namespace std;


TEST(UTF8DecoderTest, ASCIICodePoints) {
	utf8::Decoder decoder;

	EXPECT_EQ(U'\x0000', decoder.decode('\x00'));
	EXPECT_EQ(U'\u0020', decoder.decode('\x20'));
	EXPECT_EQ(U'\u0024', decoder.decode('\x24'));
	EXPECT_EQ(U'\u007F', decoder.decode('\x7F'));
}
