#include "test-utf16.hpp"
using namespace unicode;
using namespace std;

enum : char16_t {
	BMP_CODEPOINT   = u'\x0064',
	LEAD_SURROGATE  = u'\xD800',
	TRAIL_SURROGATE = u'\xDC00',
};

TEST_F(UTF16DecoderTest, UnexpectedTrailSurrogate) {
	// At the beginning of the decoding
	TEST_DECODE_WITH_FAILURE(utf16::UnexpectedTrailSurrogate, NO_CODE_POINT, TRAIL_SURROGATE);

	// During decoding
	TEST_DECODE_WITH_FAILURE(utf16::UnexpectedTrailSurrogate, NO_CODE_POINT, TRAIL_SURROGATE);
}

TEST_F(UTF16DecoderTest, ExpectedTrailSurrogate) {
	TEST_DECODE_WITH_FAILURE(utf16::ExpectedTrailSurrogate, NO_CODE_POINT, LEAD_SURROGATE, BMP_CODEPOINT);
	TEST_DECODE_WITH_FAILURE(utf16::ExpectedTrailSurrogate, NO_CODE_POINT, LEAD_SURROGATE, LEAD_SURROGATE);
}

TEST_F(UTF16DecoderTest, Partial) {
	EXPECT_FALSE(decoder.partial());

	SIMPLE_DECODE_CHECK();
	EXPECT_FALSE(decoder.partial());

	decoder.decode(LEAD_SURROGATE);
	EXPECT_TRUE(decoder.partial());

	decoder.decode(TRAIL_SURROGATE);
	EXPECT_FALSE(decoder.partial());

	SIMPLE_DECODE_CHECK();
	EXPECT_FALSE(decoder.partial());
}

TEST_F(UTF16DecoderTest, Reset) {
	decoder.decode(LEAD_SURROGATE);
	EXPECT_TRUE(decoder.partial());

	decoder.reset();
	EXPECT_FALSE(decoder.partial());
	EXPECT_THROW(decoder.decode(TRAIL_SURROGATE), utf16::UnexpectedTrailSurrogate);
	EXPECT_FALSE(decoder.partial());

	SIMPLE_DECODE_CHECK();
}

TEST_F(UTF16DecoderTest, Polymorphic) {
	utf16::PolymorphicDecoder utf16Decoder;
	::unicode::Encoding<char16_t, 2>::Decoder* decoder = &utf16Decoder;

	EXPECT_FALSE(decoder->partial());

	EXPECT_EQ(U'@', decoder->decode(u'@'));
	EXPECT_FALSE(decoder->partial());

	EXPECT_EQ(utf16::PartiallyDecoded, decoder->decode(LEAD_SURROGATE));
	EXPECT_TRUE(decoder->partial());

	char32_t expectedCodePoint = ((( LEAD_SURROGATE - u'\xD800') << 10)
	                           |   (TRAIL_SURROGATE - u'\xDC00'))
	                           + 0x010000;

	EXPECT_EQ(expectedCodePoint, decoder->decode(TRAIL_SURROGATE));
	EXPECT_FALSE(decoder->partial());

	EXPECT_EQ(utf16::PartiallyDecoded, decoder->decode(LEAD_SURROGATE));
	decoder->reset();
	EXPECT_FALSE(decoder->partial());
}
