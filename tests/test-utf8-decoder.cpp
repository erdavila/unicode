#include "test-utf8.hpp"
using namespace unicode;
using namespace std;

TEST_F(UTF8DecoderTest, InvalidCodePoints) {
	// 11110|100, 10|010000, 10|000000, 10|000000 -> U+ ---100|01 0000|0000 00|000000
	TEST_INVALID_CODE_POINT_DECODE(U'\U00110000', '\xF4', '\x90', '\x80', '\x80');

	// 11110|100, 10|111111, 10|111111, 10|111111 -> U+ ---100|11 1111|1111 11|111111
	TEST_INVALID_CODE_POINT_DECODE(U'\U0013FFFF', '\xF4', '\xBF', '\xBF', '\xBF');
}

TEST_F(UTF8DecoderTest, OverlongEncodings) {
	// 110|00001, 10|111111 -> U+ -----000 01|111111
	TEST_DECODE_WITH_FAILURE(utf8::OverlongEncoding, U'\u007F', '\xC1', '\xBF');

	// 1110|0000, 10|011111, 10|111111 -> U+ 0000|0111 11|111111
	TEST_DECODE_WITH_FAILURE(utf8::OverlongEncoding, U'\u07FF', '\xE0', '\x9F', '\xBF');

	// 11110|000, 10|001111, 10|111111, 10|111111 -> U+ 1111|1111 11|111111
	TEST_DECODE_WITH_FAILURE(utf8::OverlongEncoding, U'\uFFFF', '\xF0', '\x8F', '\xBF', '\xBF');
}

enum : char {
	ASCII_BYTE        = '\x00', // 0-------
	CONTINUATION_BYTE = '\x80', // 10------
	LEADING3_BYTE     = '\xEF', // 1110----
	INVALID_BYTE      = '\xFF',
};

TEST_F(UTF8DecoderTest, UnexpectedContinuationByte) {
	// At the begging of the decoding
	TEST_DECODE_WITH_FAILURE(utf8::UnexpectedContinuationByte, NO_CODE_POINT, CONTINUATION_BYTE);

	// During decoding
	TEST_DECODE_WITH_FAILURE(utf8::UnexpectedContinuationByte, NO_CODE_POINT, CONTINUATION_BYTE);
}

TEST_F(UTF8DecoderTest, ExpectedContinuationByte) {
	TEST_DECODE_WITH_FAILURE(utf8::ExpectedContinuationByte, NO_CODE_POINT, LEADING3_BYTE, ASCII_BYTE);
	TEST_DECODE_WITH_FAILURE(utf8::ExpectedContinuationByte, NO_CODE_POINT, LEADING3_BYTE, LEADING3_BYTE);
}

TEST_F(UTF8DecoderTest, InvalidByte) {
	// At the begging of the decoding
	TEST_DECODE_WITH_FAILURE(utf8::InvalidByte, NO_CODE_POINT, INVALID_BYTE);

	// During decoding
	TEST_DECODE_WITH_FAILURE(utf8::InvalidByte, NO_CODE_POINT, INVALID_BYTE);

	// When expecting a continuation byte
	TEST_DECODE_WITH_FAILURE(utf8::InvalidByte, NO_CODE_POINT, LEADING3_BYTE, INVALID_BYTE);
}

TEST_F(UTF8DecoderTest, Partial) {
	EXPECT_FALSE(decoder.partial());

	SIMPLE_DECODE_CHECK();
	EXPECT_FALSE(decoder.partial());

	decoder.decode(LEADING3_BYTE);
	EXPECT_TRUE(decoder.partial());

	decoder.decode(CONTINUATION_BYTE);
	EXPECT_TRUE(decoder.partial());

	decoder.decode(CONTINUATION_BYTE);
	EXPECT_FALSE(decoder.partial());

	SIMPLE_DECODE_CHECK();
	EXPECT_FALSE(decoder.partial());
}

TEST_F(UTF8DecoderTest, Reset) {
	decoder.decode(LEADING3_BYTE);
	EXPECT_TRUE(decoder.partial());

	decoder.reset();
	EXPECT_FALSE(decoder.partial());
	EXPECT_THROW(decoder.decode(CONTINUATION_BYTE), utf8::UnexpectedContinuationByte);

	SIMPLE_DECODE_CHECK();

	decoder.decode(LEADING3_BYTE);
	decoder.decode(CONTINUATION_BYTE);

	EXPECT_TRUE(decoder.partial());

	decoder.reset();
	EXPECT_FALSE(decoder.partial());
}

TEST_F(UTF8DecoderTest, Polymorphic) {
	utf8::PolymorphicDecoder utf8Decoder;
	::unicode::Encoding<char, 4>::Decoder* decoder = &utf8Decoder;

	EXPECT_FALSE(decoder->partial());

	EXPECT_EQ(U'@', decoder->decode('@'));
	EXPECT_FALSE(decoder->partial());

	EXPECT_EQ(utf8::PartiallyDecoded, decoder->decode(LEADING3_BYTE));
	EXPECT_TRUE(decoder->partial());

	EXPECT_EQ(utf8::PartiallyDecoded, decoder->decode(CONTINUATION_BYTE));
	EXPECT_TRUE(decoder->partial());

	char32_t expectedCodePoint = ((    LEADING3_BYTE & 0x0F) << 12)
	                           | ((CONTINUATION_BYTE & 0x3F) <<  6)
	                           |  (CONTINUATION_BYTE & 0x3F);

	EXPECT_EQ(expectedCodePoint, decoder->decode(CONTINUATION_BYTE));
	EXPECT_FALSE(decoder->partial());

	EXPECT_EQ(utf8::PartiallyDecoded, decoder->decode(LEADING3_BYTE));
	decoder->reset();
	EXPECT_FALSE(decoder->partial());
}
