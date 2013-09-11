#include "test-utf16be.hpp"
using namespace unicode;
using namespace std;

enum : char {
	BMP_CODEPOINT_HIGH   = '\x00',
	BMP_CODEPOINT_LOW    = '\x64',
	LEAD_SURROGATE_HIGH  = '\xD8',
	LEAD_SURROGATE_LOW   = '\x00',
	TRAIL_SURROGATE_HIGH = '\xDC',
	TRAIL_SURROGATE_LOW  = '\x00',
};

TEST_F(UTF16BEDecoderTest, UnexpectedTrailSurrogate) {
	// At the beginning of the decoding
	TEST_DECODE_WITH_FAILURE(utf16be::UnexpectedTrailSurrogate, NO_CODE_POINT, TRAIL_SURROGATE_HIGH, TRAIL_SURROGATE_LOW);

	// During decoding
	TEST_DECODE_WITH_FAILURE(utf16be::UnexpectedTrailSurrogate, NO_CODE_POINT, TRAIL_SURROGATE_HIGH, TRAIL_SURROGATE_LOW);
}

TEST_F(UTF16BEDecoderTest, ExpectedTrailSurrogate) {
	TEST_DECODE_WITH_FAILURE(utf16be::ExpectedTrailSurrogate, NO_CODE_POINT, LEAD_SURROGATE_HIGH, LEAD_SURROGATE_LOW, BMP_CODEPOINT_HIGH, BMP_CODEPOINT_LOW);
	TEST_DECODE_WITH_FAILURE(utf16be::ExpectedTrailSurrogate, NO_CODE_POINT, LEAD_SURROGATE_HIGH, LEAD_SURROGATE_LOW, LEAD_SURROGATE_HIGH, LEAD_SURROGATE_LOW);
}

TEST_F(UTF16BEDecoderTest, Partial) {
	EXPECT_FALSE(decoder.partial());

	SIMPLE_DECODE_CHECK();
	EXPECT_FALSE(decoder.partial());

	decoder.decode(LEAD_SURROGATE_HIGH);
	EXPECT_TRUE(decoder.partial());

	decoder.decode(LEAD_SURROGATE_LOW);
	EXPECT_TRUE(decoder.partial());

	decoder.decode(TRAIL_SURROGATE_HIGH);
	EXPECT_TRUE(decoder.partial());

	decoder.decode(TRAIL_SURROGATE_LOW);
	EXPECT_FALSE(decoder.partial());

	SIMPLE_DECODE_CHECK();
	EXPECT_FALSE(decoder.partial());
}

TEST_F(UTF16BEDecoderTest, Reset1) {
	decoder.decode(LEAD_SURROGATE_HIGH);
	EXPECT_TRUE(decoder.partial());

	decoder.reset();
	EXPECT_FALSE(decoder.partial());
	decoder.decode(BMP_CODEPOINT_HIGH);
	EXPECT_TRUE(decoder.partial());
	decoder.decode(BMP_CODEPOINT_LOW);
	EXPECT_FALSE(decoder.partial());

	SIMPLE_DECODE_CHECK();
}

TEST_F(UTF16BEDecoderTest, Reset2) {
	decoder.decode(LEAD_SURROGATE_HIGH);
	EXPECT_TRUE(decoder.partial());
	decoder.decode(LEAD_SURROGATE_LOW);
	EXPECT_TRUE(decoder.partial());

	decoder.reset();
	EXPECT_FALSE(decoder.partial());
	decoder.decode(BMP_CODEPOINT_HIGH);
	EXPECT_TRUE(decoder.partial());
	decoder.decode(BMP_CODEPOINT_LOW);
	EXPECT_FALSE(decoder.partial());

	SIMPLE_DECODE_CHECK();
}


TEST_F(UTF16BEDecoderTest, Reset3) {
	decoder.decode(LEAD_SURROGATE_HIGH);
	EXPECT_TRUE(decoder.partial());
	decoder.decode(LEAD_SURROGATE_LOW);
	EXPECT_TRUE(decoder.partial());

	decoder.reset();
	EXPECT_FALSE(decoder.partial());
	decoder.decode(TRAIL_SURROGATE_HIGH);
	EXPECT_TRUE(decoder.partial());
	EXPECT_THROW(decoder.decode(TRAIL_SURROGATE_LOW), utf16be::UnexpectedTrailSurrogate);
	EXPECT_FALSE(decoder.partial());

	SIMPLE_DECODE_CHECK();
}


TEST_F(UTF16BEDecoderTest, Reset4) {
	decoder.decode(LEAD_SURROGATE_HIGH);
	EXPECT_TRUE(decoder.partial());
	decoder.decode(LEAD_SURROGATE_LOW);
	EXPECT_TRUE(decoder.partial());
	decoder.decode(TRAIL_SURROGATE_HIGH);
	EXPECT_TRUE(decoder.partial());

	decoder.reset();
	EXPECT_FALSE(decoder.partial());
	decoder.decode(BMP_CODEPOINT_HIGH);
	EXPECT_TRUE(decoder.partial());
	decoder.decode(BMP_CODEPOINT_LOW);
	EXPECT_FALSE(decoder.partial());

	SIMPLE_DECODE_CHECK();
}

TEST_F(UTF16BEDecoderTest, Polymorphic) {
	utf16be::PolymorphicDecoder utf16beDecoder;
	::unicode::Encoding<char, 4>::Decoder* decoder = &utf16beDecoder;

	EXPECT_FALSE(decoder->partial());

	EXPECT_EQ(utf16be::PartiallyDecoded, decoder->decode('\0'));
	EXPECT_TRUE(decoder->partial());
	EXPECT_EQ(U'@', decoder->decode('@'));
	EXPECT_FALSE(decoder->partial());

	EXPECT_EQ(utf16::PartiallyDecoded, decoder->decode(LEAD_SURROGATE_HIGH));
	EXPECT_TRUE(decoder->partial());
	EXPECT_EQ(utf16::PartiallyDecoded, decoder->decode(LEAD_SURROGATE_LOW));
	EXPECT_TRUE(decoder->partial());

	char32_t expectedCodePoint = (
		[]() -> char32_t {
			utf16::Decoder utf16Decoder;
			char32_t codePoint;
			codePoint = utf16Decoder.decode(( LEAD_SURROGATE_HIGH << 8) | (0xFF & LEAD_SURROGATE_LOW));
			assert(codePoint == utf16::PartiallyDecoded);
			codePoint = utf16Decoder.decode((TRAIL_SURROGATE_HIGH << 8) | (0xFF & TRAIL_SURROGATE_LOW));
			assert(codePoint != utf16::PartiallyDecoded);
			return codePoint;
		}
	)();

	EXPECT_EQ(utf16::PartiallyDecoded, decoder->decode(TRAIL_SURROGATE_HIGH));
	EXPECT_TRUE(decoder->partial());
	EXPECT_EQ(expectedCodePoint, decoder->decode(TRAIL_SURROGATE_LOW));
	EXPECT_FALSE(decoder->partial());

	EXPECT_EQ(utf16::PartiallyDecoded, decoder->decode(LEAD_SURROGATE_HIGH));
	decoder->reset();
	EXPECT_FALSE(decoder->partial());
}
