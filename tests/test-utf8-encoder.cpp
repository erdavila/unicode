#include "unicode/unicode.hpp"
#include "gtest/gtest.h"
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
			     " is 0x" << hex << (int)codeUnits[i] << " instead"
			     " of 0x" << hex << (int)expectedCodeUnits[i];
		}
	}

	return ::testing::AssertionSuccess();
}

TEST(UTF8EncoderTest, ASCIICodePoints) {
	EXPECT_TRUE(encodes(U'\x00', {'\x00'}));
	EXPECT_TRUE(encodes(U'\u0020', {'\x20'}));
	EXPECT_TRUE(encodes(U'\u007F', {'\x7F'}));
}
