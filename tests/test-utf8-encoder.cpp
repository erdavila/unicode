#include "gtest/gtest.h"
#include "test.hpp"
using namespace unicode;
using namespace std;

::testing::AssertionResult encodes(char32_t codePoint, const vector<byte>& expectedCodeUnits) {
	utf8::Encoder encoder;
	utf8::CodeUnits codeUnits;
	utf8::CodeUnitsCount codeUnitsCount;

	encoder.encode(codePoint, codeUnits, codeUnitsCount);

	if(codeUnitsCount != expectedCodeUnits.size()) {
		return ::testing::AssertionFailure() << codeUnitsCount << " code units instead of " << expectedCodeUnits.size();
	}
	for(auto i = 0u; i < codeUnitsCount; i++) {
		if(codeUnits[i] != expectedCodeUnits[i]) {
			return ::testing::AssertionFailure() << "code unit at position " << i <<
			     " is 0x" << to_hex(codeUnits[i], 2) << " instead"
			     " of 0x" << to_hex(expectedCodeUnits[i], 2);
		}
	}
	return ::testing::AssertionSuccess();
}

TEST(UTF8EncoderTest, ASCIICodePoints) {
	// -|0000000 -> 0|0000000
	EXPECT_TRUE(encodes(U'\x00', bytes{'\x00'}));

	// -|0100000 -> 0|0100000
	EXPECT_TRUE(encodes(U'\u0020', bytes{'\x20'}));

	// -|0100100 -> 0|0100100
	EXPECT_TRUE(encodes(U'\u0024', bytes{'\x24'}));

	// -|1111111 -> 0|1111111
	EXPECT_TRUE(encodes(U'\u007F', bytes{'\x7F'}));
}

TEST(UTF8EncoderTest, CodePointsEncodedToTwoBytes) {
	// -----|000 10|000000 -> 110|00010, 10|000000
	EXPECT_TRUE(encodes(U'\u0080', bytes{'\xC2', '\x80'}));

	// -----|000 10|100010 -> 110|00010, 10|100010
	EXPECT_TRUE(encodes(U'\u00A2', bytes{'\xC2', '\xA2'}));

	// -----|010 10|101010 -> 110|01010, 10|101010
	EXPECT_TRUE(encodes(U'\u02AA', bytes{'\xCA', '\xAA'}));

	// -----|111 11|111111 -> 110|11111, 10|111111
	EXPECT_TRUE(encodes(U'\u07FF', bytes{'\xDF', '\xBF'}));
}

TEST(UTF8EncoderTest, CodePointsEncodedToThreeBytes) {
	// 0000|1000 00|000000 -> 1110|0000, 10|100000, 10|000000
	EXPECT_TRUE(encodes(U'\u0800', bytes{'\xE0', '\xA0', '\x80'}));

	// 0010|0000 10|101100 -> 1110|0010, 10|000010, 10|101100
	EXPECT_TRUE(encodes(U'\u20AC', bytes{'\xE2', '\x82', '\xAC'}));

	// 1111|1110 11|111111 -> 1110|1111, 10|111011, 10|111111
	EXPECT_TRUE(encodes(U'\uFEFF', bytes{'\xEF', '\xBB', '\xBF'}));

	// 1111|1111 11|111111 -> 1110|1111, 10|111111, 10|111111
	EXPECT_TRUE(encodes(U'\uFFFF', bytes{'\xEF', '\xBF', '\xBF'}));
}

TEST(UTF8EncoderTest, CodePointsEncodedToFourBytes) {
	// ---000|01 0000|0000 00|000000 -> 11110|000, 10|010000, 10|000000, 10|000000
	EXPECT_TRUE(encodes(U'\U00010000', bytes{'\xF0', '\x90', '\x80', '\x80'}));

	// ---000|01 1101|0001 00|011110 -> 11110|000, 10|011101, 10|000100, 10|011110
	EXPECT_TRUE(encodes(U'\U0001D11E', bytes{'\xF0', '\x9D', '\x84', '\x9E'}));

	// ---000|10 0100|1011 01|100010 -> 11110|000, 10|100100, 10|101101, 10|100010
	EXPECT_TRUE(encodes(U'\U00024B62', bytes{'\xF0', '\xA4', '\xAD', '\xA2'}));

	// ---100|00 1111|1111 11|111111 -> 11110|100, 10|001111, 10|111111, 10|111111
	EXPECT_TRUE(encodes(U'\U0010FFFF', bytes{'\xF4', '\x8F', '\xBF', '\xBF'}));
}

::testing::AssertionResult failsToEncode(char32_t codePoint) {
	utf8::Encoder encoder;
	utf8::CodeUnits codeUnits;
	utf8::CodeUnitsCount codeUnitsCount;

	try {
		encoder.encode(codePoint, codeUnits, codeUnitsCount);
		return ::testing::AssertionFailure() << "Exception not thrown";
	} catch(utf8::InvalidCodePoint& e) {
		if(e.codePoint != codePoint) {
			return ::testing::AssertionFailure() << "Wrong code point in exception: U+" << to_hex(e.codePoint, 4);
		}
		return ::testing::AssertionSuccess() << "Exception thrown: " << e.what();
	}
}

TEST(UTF8EncoderTest, InvalidCodePoints) {
	EXPECT_TRUE(failsToEncode(U'\U00110000'));
	EXPECT_TRUE(failsToEncode(U'\U7FFFFFFF'));
	EXPECT_TRUE(failsToEncode(U'\x80000000'));
	EXPECT_TRUE(failsToEncode(U'\xFFFFFFFF'));
}
