#include "test-utf8.hpp"
using namespace std;
using namespace unicode;

class UTF8Test : public EncodingTest<UTF8EncoderTest, UTF8DecoderTest> {
protected:

};

ostream& operator<<(ostream& os, utf8::ByteType byteType) {
#define CASE(BT) case BT: return os << #BT;
	switch(byteType) {
	CASE(utf8::ByteType::ASCII);
	CASE(utf8::ByteType::CONTINUATION);
	CASE(utf8::ByteType::LEADING2);
	CASE(utf8::ByteType::LEADING3);
	CASE(utf8::ByteType::LEADING4);
	CASE(utf8::ByteType::INVALID);
	default: return os << "????";
	}
#undef CASE
}

void testByteTypeRange(utf8::ByteType expectedType, int minValue, int maxValue) {
	for(int i = minValue; i <= maxValue; i++) {
		char b = i;
		EXPECT_EQ(expectedType, utf8::byteType(b)) << "Wrong type for byte '\\x" << to_hex(i, 2) << "'";
	}
}

TEST_F(UTF8Test, ByteTypes) {
	testByteTypeRange(utf8::ByteType::ASCII,        0x00, 0x7F); // 0-------
	testByteTypeRange(utf8::ByteType::CONTINUATION, 0x80, 0xBF); // 10------
	testByteTypeRange(utf8::ByteType::LEADING2,     0xC0, 0xDF); // 110-----
	testByteTypeRange(utf8::ByteType::LEADING3,     0xE0, 0xEF); // 1110----
	testByteTypeRange(utf8::ByteType::LEADING4,     0xF0, 0xF4); // 11110--- (up to 11110100)
	testByteTypeRange(utf8::ByteType::INVALID,      0xF5, 0xFF);
}

TEST_F(UTF8Test, CodePointsEncodedToOneByte) {
	// U+ -|0000000 <-> 0|0000000
	TEST_CODE_POINT_ENCODING(U'\x0000', '\x00');

	// U+ -|0100000 <-> 0|0100000
	TEST_CODE_POINT_ENCODING(U'\u0020', '\x20');

	// U+ -|0100100 <-> 0|0100100
	TEST_CODE_POINT_ENCODING(U'\u0024', '\x24');

	// U+ -|1111111 <-> 0|1111111
	TEST_CODE_POINT_ENCODING(U'\u007F', '\x7F');
}

TEST_F(UTF8Test, CodePointsEncodedToTwoBytes) {
	// U+ -----|000 10|000000 <-> 110|00010, 10|000000
	TEST_CODE_POINT_ENCODING(U'\u0080', '\xC2', '\x80');

	// U+ -----|000 10|100010 <-> 110|00010, 10|100010
	TEST_CODE_POINT_ENCODING(U'\u00A2', '\xC2', '\xA2');

	// U+ -----|010 10|101010 <-> 110|01010, 10|101010
	TEST_CODE_POINT_ENCODING(U'\u02AA', '\xCA', '\xAA');

	// U+ -----|111 11|111111 <-> 110|11111, 10|111111
	TEST_CODE_POINT_ENCODING(U'\u07FF', '\xDF', '\xBF');
}

TEST_F(UTF8Test, CodePointsEncodedToThreeBytes) {
	// U+ 0000|1000 00|000000 <-> 1110|0000, 10|100000, 10|000000
	TEST_CODE_POINT_ENCODING(U'\u0800', '\xE0', '\xA0', '\x80');

	// U+ 0010|0000 10|101100 <-> 1110|0010, 10|000010, 10|101100
	TEST_CODE_POINT_ENCODING(U'\u20AC', '\xE2', '\x82', '\xAC');

	// U+ 1111|1110 11|111111 <-> 1110|1111, 10|111011, 10|111111
	TEST_CODE_POINT_ENCODING(U'\uFEFF', '\xEF', '\xBB', '\xBF');

	// U+ 1111|1111 11|111111 <-> 1110|1111, 10|111111, 10|111111
	TEST_CODE_POINT_ENCODING(U'\uFFFF', '\xEF', '\xBF', '\xBF');
}

TEST_F(UTF8Test, CodePointsEncodedToFourBytes) {
	// U+ ---000|01 0000|0000 00|000000 <-> 11110|000, 10|010000, 10|000000, 10|000000
	TEST_CODE_POINT_ENCODING(U'\U00010000', '\xF0', '\x90', '\x80', '\x80');

	// U+ ---000|01 1101|0001 00|011110 <-> 11110|000, 10|011101, 10|000100, 10|011110
	TEST_CODE_POINT_ENCODING(U'\U0001D11E', '\xF0', '\x9D', '\x84', '\x9E');

	// U+ ---000|10 0100|1011 01|100010 <-> 11110|000, 10|100100, 10|101101, 10|100010
	TEST_CODE_POINT_ENCODING(U'\U00024B62', '\xF0', '\xA4', '\xAD', '\xA2');

	// U+ ---100|00 1111|1111 11|111111 <-> 11110|100, 10|001111, 10|111111, 10|111111
	TEST_CODE_POINT_ENCODING(U'\U0010FFFF', '\xF4', '\x8F', '\xBF', '\xBF');
}
