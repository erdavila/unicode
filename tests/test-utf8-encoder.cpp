#include "unicode/unicode.hpp"
#include "gtest/gtest.h"
#include <initializer_list>
#include <vector>
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
			     " is " << (int)codeUnits[i] << " instead"
			     " of " << (int)expectedCodeUnits[i];
		}
	}
	return ::testing::AssertionSuccess();
}

struct bytes : vector<byte> {
	bytes(initializer_list<char> lst) : vector<byte>(lst.begin(), lst.end()) {}
};

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
