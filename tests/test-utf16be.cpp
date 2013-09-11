#include "test-utf16be.hpp"
using namespace unicode;
using namespace std;

class UTF16BETest : public EncodingTest<UTF16BEEncoderTest, UTF16BEDecoderTest> {};

TEST_F(UTF16BETest, Encoding) {
	TEST_CODE_POINT_ENCODING(U'\x0000', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\u007A', '\x00', '\x7A');
	TEST_CODE_POINT_ENCODING(U'\u6C34', '\x6C', '\x34');
	TEST_CODE_POINT_ENCODING(U'\uFEFF', '\xFE', '\xFF');
	TEST_CODE_POINT_ENCODING(U'\uFFFF', '\xFF', '\xFF');
	TEST_CODE_POINT_ENCODING(U'\U00010000', '\xD8', '\x00', '\xDC', '\x00');
	TEST_CODE_POINT_ENCODING(U'\U0001D11E', '\xD8', '\x34', '\xDD', '\x1E');
	TEST_CODE_POINT_ENCODING(U'\U00024B62', '\xD8', '\x52', '\xDF', '\x62');
	TEST_CODE_POINT_ENCODING(U'\U0010FFFF', '\xDB', '\xFF', '\xDF', '\xFF');
}
