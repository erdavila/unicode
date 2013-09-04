#include "test.hpp"
using namespace unicode;
using namespace std;


TEST(UTF8DecoderTest, ASCIICodePoints) {
	utf8::Decoder decoder;

	EXPECT_EQ(U'\x0000', decoder.decode('\x00'));
	EXPECT_EQ(U'\u0020', decoder.decode('\x20'));
	EXPECT_EQ(U'\u0024', decoder.decode('\x24'));
	EXPECT_EQ(U'\u007F', decoder.decode('\x7F'));
}

::testing::AssertionResult decodes(vector<byte> codeUnits, char32_t expectedCodePoint) {
	utf8::Decoder decoder;

	const auto lastIndex = codeUnits.size() - 1;
	for(auto index = 0u; index <= lastIndex ; index++) {
		byte codeUnit = codeUnits[index];
		char32_t codePoint = decoder.decode(codeUnit);

		if(index == lastIndex) {
			if(codePoint != expectedCodePoint) {
				return ::testing::AssertionFailure()
				       << "expected code point U+" << to_hex(expectedCodePoint, 4)
				       << " but got U+" << to_hex(codePoint,4);
			}
		} else {
			if(codePoint != utf8::PartiallyDecoded) {
				return ::testing::AssertionFailure()
				       << "expected partially decoded code point but got U+" << to_hex(codePoint,4)
				       << " at index " << index;
			}
		}
	}

	return ::testing::AssertionSuccess();
}

TEST(UTF8DecoderTest, CodePointsEncodedToTwoBytes) {
	// 110|00010, 10|000000 -> -----|000 10|000000
	EXPECT_TRUE(decodes(bytes{'\xC2', '\x80'}, U'\u0080'));

	// 110|00010, 10|100010 -> -----|000 10|100010
	EXPECT_TRUE(decodes(bytes{'\xC2', '\xA2'}, U'\u00A2'));

	// 110|01010, 10|101010 -> -----|010 10|101010
	EXPECT_TRUE(decodes(bytes{'\xCA', '\xAA'}, U'\u02AA'));

	// 110|11111, 10|111111 -> -----|111 11|111111
	EXPECT_TRUE(decodes(bytes{'\xDF', '\xBF'}, U'\u07FF'));
}

TEST(UTF8DecoderTest, CodePointsEncodedToThreeBytes) {
	// 1110|0000, 10|100000, 10|000000 -> 0000|1000 00|000000
	EXPECT_TRUE(decodes(bytes{'\xE0', '\xA0', '\x80'}, U'\u0800'));

	// 1110|0010, 10|000010, 10|101100 -> 0010|0000 10|101100
	EXPECT_TRUE(decodes(bytes{'\xE2', '\x82', '\xAC'}, U'\u20AC'));

	// 1110|1111, 10|111011, 10|111111 -> 1111|1110 11|111111
	EXPECT_TRUE(decodes(bytes{'\xEF', '\xBB', '\xBF'}, U'\uFEFF'));

	// 1110|1111, 10|111111, 10|111111 -> 1111|1111 11|111111
	EXPECT_TRUE(decodes(bytes{'\xEF', '\xBF', '\xBF'}, U'\uFFFF'));
}
