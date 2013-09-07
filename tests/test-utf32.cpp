#include "test.hpp"
using namespace unicode;
using namespace std;

namespace /*unnamed*/ {

::testing::AssertionResult encodes(const char32_t codePoint) {
	utf32::Encoder encoder;
	utf32::CodeUnits codeUnits;
	utf32::CodeUnitsCount codeUnitsCount = encoder.encode(codePoint, codeUnits);
	if(codeUnitsCount != 1) {
		return ::testing::AssertionFailure() << "Encoded to " << codeUnitsCount << " code units";
	}
	utf32::CodeUnit codeUnit = codeUnits[0];
	if(codeUnit != codePoint) {
		return ::testing::AssertionFailure() << "Encoded as 0x" << to_hex(codeUnit, 2);
	}

	utf32::Decoder decoder;
	char32_t decodedCodePoint = decoder.decode(codeUnit);
	if(decodedCodePoint != codePoint) {
		return ::testing::AssertionFailure() << "Decoded as U+" << to_hex(decodedCodePoint, 4);
	}

	return ::testing::AssertionSuccess();
}

TEST(UTF32, Encoding) {
	EXPECT_TRUE(encodes(U'\x0000'));
	EXPECT_TRUE(encodes(U'\u0020'));
	EXPECT_TRUE(encodes(U'\u0024'));
	EXPECT_TRUE(encodes(U'\u007F'));
	EXPECT_TRUE(encodes(U'\u0080'));
	EXPECT_TRUE(encodes(U'\u00A2'));
	EXPECT_TRUE(encodes(U'\u02AA'));
	EXPECT_TRUE(encodes(U'\u07FF'));
	EXPECT_TRUE(encodes(U'\u0800'));
	EXPECT_TRUE(encodes(U'\u20AC'));
	EXPECT_TRUE(encodes(U'\uFEFF'));
	EXPECT_TRUE(encodes(U'\uFFFF'));
	EXPECT_TRUE(encodes(U'\U00010000'));
	EXPECT_TRUE(encodes(U'\U0001D11E'));
	EXPECT_TRUE(encodes(U'\U00024B62'));
	EXPECT_TRUE(encodes(U'\U0010FFFF'));
}

::testing::AssertionResult failsToEncode(char32_t codePoint) {
	utf32::Encoder encoder;
	utf32::CodeUnits codeUnits;
	try {
		encoder.encode(codePoint, codeUnits);
		return ::testing::AssertionFailure() << "Exception not thrown during encode";
	} catch(utf32::InvalidCodePoint& e) {
		if(e.codePoint != codePoint) {
			return ::testing::AssertionFailure() << "Wrong code point in exception: U+" << to_hex(e.codePoint, 4);
		}
	}

	utf32::Decoder decoder;
	utf32::CodeUnit codeUnit = codePoint;
	try {
		decoder.decode(codeUnit);
		return ::testing::AssertionFailure() << "Exception not thrown during decode";
	} catch(utf32::InvalidCodeUnit& e) {
		;
	}

	return ::testing::AssertionSuccess();
}

TEST(UTF32, InvalidCodePoints) {
	EXPECT_TRUE(failsToEncode(U'\U00110000'));
	EXPECT_TRUE(failsToEncode(U'\U7FFFFFFF'));
	EXPECT_TRUE(failsToEncode(U'\x80000000'));
	EXPECT_TRUE(failsToEncode(U'\xFFFFFFFF'));
}

TEST(UTF32, Polymorphic) {
	using EncodingBase = Encoding<char32_t, 1>;

	utf32::PolymorphicEncoder utf32Encoder;
	EncodingBase::Encoder* encoder = &utf32Encoder;
	EncodingBase::CodeUnits codeUnits;
	EncodingBase::CodeUnitsCount codeUnitsCount;
	codeUnitsCount = encoder->encode(U'@', codeUnits);
	ASSERT_EQ(1u, codeUnitsCount);
	EXPECT_EQ(U'@', codeUnits[0]);

	utf32::PolymorphicDecoder utf32Decoder;
	EncodingBase::Decoder* decoder = &utf32Decoder;
	EXPECT_EQ(U'@', decoder->decode(U'@'));
}

}
