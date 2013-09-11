#include "test-utf16le.hpp"
using namespace unicode;
using namespace std;

class UTF16LETest : public EncodingTest<UTF16LEEncoderTest, UTF16LEDecoderTest> {};

TEST_F(UTF16LETest, Encoding) {
	TEST_CODE_POINT_ENCODING(U'\x0000', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\u007A', '\x7A', '\x00');
	TEST_CODE_POINT_ENCODING(U'\u6C34', '\x34', '\x6C');
	TEST_CODE_POINT_ENCODING(U'\uFEFF', '\xFF', '\xFE');
	TEST_CODE_POINT_ENCODING(U'\uFFFF', '\xFF', '\xFF');
	TEST_CODE_POINT_ENCODING(U'\U00010000', '\x00', '\xD8', '\x00', '\xDC');
	TEST_CODE_POINT_ENCODING(U'\U0001D11E', '\x34', '\xD8', '\x1E', '\xDD');
	TEST_CODE_POINT_ENCODING(U'\U00024B62', '\x52', '\xD8', '\x62', '\xDF');
	TEST_CODE_POINT_ENCODING(U'\U0010FFFF', '\xFF', '\xDB', '\xFF', '\xDF');
}
