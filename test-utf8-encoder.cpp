#include "unicode.hpp"
#include "gtest/gtest.h"
using namespace unicode;

TEST(UTF8EncoderTest, ASCIICodePoints) {
	utf8::Encoder encoder;
	utf8::CodeUnits codeUnits;
	utf8::CodeUnitsCount codeUnitsCount;

	encoder.encode(U'\x00', codeUnits, codeUnitsCount);
	EXPECT_EQ(1u, codeUnitsCount);
	EXPECT_EQ('\x00', codeUnits[0]);

	encoder.encode(U'\u0020', codeUnits, codeUnitsCount);
	EXPECT_EQ(1u, codeUnitsCount);
	EXPECT_EQ('\x20', codeUnits[0]);

	encoder.encode(U'\u007F', codeUnits, codeUnitsCount);
	EXPECT_EQ(1u, codeUnitsCount);
	EXPECT_EQ('\x7F', codeUnits[0]);
}
