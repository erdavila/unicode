#include "test-utf16.hpp"
using namespace std;
using namespace unicode;

class UTF16Test : public EncodingTest<UTF16EncoderTest, UTF16DecoderTest> {
protected:

};

TEST_F(UTF16Test, LeadSurrogate) {
	EXPECT_FALSE(utf16::isLeadSurrogate(u'\x0000'));
	EXPECT_FALSE(utf16::isLeadSurrogate(u'\uD7FF'));
	EXPECT_TRUE( utf16::isLeadSurrogate(u'\xD800'));
	EXPECT_TRUE( utf16::isLeadSurrogate(u'\xDBFF'));
	EXPECT_FALSE(utf16::isLeadSurrogate(u'\xDC00'));
	EXPECT_FALSE(utf16::isLeadSurrogate(u'\xDFFF'));
	EXPECT_FALSE(utf16::isLeadSurrogate(u'\uE000'));
	EXPECT_FALSE(utf16::isLeadSurrogate(u'\xFFFF'));
}

TEST_F(UTF16Test, TrailSurrogate) {
	EXPECT_FALSE(utf16::isTrailSurrogate(u'\x0000'));
	EXPECT_FALSE(utf16::isTrailSurrogate(u'\uD7FF'));
	EXPECT_FALSE(utf16::isTrailSurrogate(u'\xD800'));
	EXPECT_FALSE(utf16::isTrailSurrogate(u'\xDBFF'));
	EXPECT_TRUE( utf16::isTrailSurrogate(u'\xDC00'));
	EXPECT_TRUE( utf16::isTrailSurrogate(u'\xDFFF'));
	EXPECT_FALSE(utf16::isTrailSurrogate(u'\uE000'));
	EXPECT_FALSE(utf16::isTrailSurrogate(u'\xFFFF'));
}

TEST_F(UTF16Test, BMPCodePoints) {
	// U+ 00000000 00000000 <-> 00000000 00000000
	TEST_CODE_POINT_ENCODING(U'\x0000', u'\x0000');

	// U+ 00000000 01111010 <-> 00000000 01111010
	TEST_CODE_POINT_ENCODING(U'\u007A', u'\u007A');

	// U+ 01101100 00111000 <-> 01101100 00111000
	TEST_CODE_POINT_ENCODING(U'\u6C34', u'\u6C34');

	// U+ 11111110 11111111 <-> 11111110 11111111
	TEST_CODE_POINT_ENCODING(U'\uFEFF', u'\xFEFF');

	// U+ 11111111 11111111 <-> 11111111 11111111
	TEST_CODE_POINT_ENCODING(U'\uFFFF', u'\xFFFF');
}

TEST_F(UTF16Test, NonBMPCodePoints) {
	// U+010000 - 0x010000 = 0x000000 = ....|0000 000000|00 00000000 <-> 110110|00 00000000, 110111|00 00000000
	TEST_CODE_POINT_ENCODING(U'\U00010000', u'\xD800', u'\xDC00');

	// U+01D11E - 0x010000 = 0x00D11E = ....|0000 110100|01 00011110 <-> 110110|00 00110100, 110111|01 00011110
	TEST_CODE_POINT_ENCODING(U'\U0001D11E', u'\xD834', u'\xDD1E');

	// U+024B62 - 0x010000 = 0x014B62 = ....|0001 010010|11 01100010 <-> 110110|00 01010010, 110111|11 01100010
	TEST_CODE_POINT_ENCODING(U'\U00024B62', u'\xD852', u'\xDF62');

	// U+10FFFF - 0x010000 = 0x0FFFFF = ....|1111 111111|11 11111111 <-> 110110|11 11111111, 110111|11 11111111
	TEST_CODE_POINT_ENCODING(U'\U0010FFFF', u'\xDBFF', u'\xDFFF');
}
