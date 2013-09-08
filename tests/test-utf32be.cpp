#include "test.hpp"
using namespace unicode;
using namespace std;

class UTF32BEEncoderTest : public EncoderTest<utf32be> {
protected:
	void simpleEncodeCheck() override {
		testEncode(U'\U0001D11E', {'\x00', '\x01', '\xD1', '\x1E'});
	}
};

class UTF32BEDecoderTest : public DecoderTest<utf32be> {
protected:
	void simpleDecodeCheck() override {
		testDecode(U'\U0001D11E', {'\x00', '\x01', '\xD1', '\x1E'});
	}
};

class UTF32BETest : public EncodingTest<UTF32BEEncoderTest, UTF32BEDecoderTest> {};

TEST_F(UTF32BETest, Encoding) {
	TEST_CODE_POINT_ENCODING(U'\x0000', '\x00', '\x00', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\u0020', '\x00', '\x00', '\x00', '\x20');
	TEST_CODE_POINT_ENCODING(U'\u0024', '\x00', '\x00', '\x00', '\x24');
	TEST_CODE_POINT_ENCODING(U'\u007F', '\x00', '\x00', '\x00', '\x7F');
	TEST_CODE_POINT_ENCODING(U'\u0080', '\x00', '\x00', '\x00', '\x80');
	TEST_CODE_POINT_ENCODING(U'\u00A2', '\x00', '\x00', '\x00', '\xA2');
	TEST_CODE_POINT_ENCODING(U'\u02AA', '\x00', '\x00', '\x02', '\xAA');
	TEST_CODE_POINT_ENCODING(U'\u07FF', '\x00', '\x00', '\x07', '\xFF');
	TEST_CODE_POINT_ENCODING(U'\u0800', '\x00', '\x00', '\x08', '\x00');
	TEST_CODE_POINT_ENCODING(U'\u20AC', '\x00', '\x00', '\x20', '\xAC');
	TEST_CODE_POINT_ENCODING(U'\uFEFF', '\x00', '\x00', '\xFE', '\xFF');
	TEST_CODE_POINT_ENCODING(U'\uFFFF', '\x00', '\x00', '\xFF', '\xFF');
	TEST_CODE_POINT_ENCODING(U'\U00010000', '\x00', '\x01', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\U0001D11E', '\x00', '\x01', '\xD1', '\x1E');
	TEST_CODE_POINT_ENCODING(U'\U00024B62', '\x00', '\x02', '\x4B', '\x62');
	TEST_CODE_POINT_ENCODING(U'\U0010FFFF', '\x00', '\x10', '\xFF', '\xFF');
}

TEST_F(UTF32BETest, InvalidCodePoints) {
	TEST_INVALID_CODE_POINT_ENCODE(U'\U00110000');
	TEST_INVALID_CODE_POINT_ENCODE(U'\U7FFFFFFF');
	TEST_INVALID_CODE_POINT_ENCODE(U'\x80000000');
	TEST_INVALID_CODE_POINT_ENCODE(U'\xFFFFFFFF');

#define TEST_INVALID_CODE_POINT_UTF32BE_DECODE(codePoint, codeUnits...) TEST_INVALID_CODE_POINT_DECODE(codePoint, codeUnits)
	TEST_INVALID_CODE_POINT_UTF32BE_DECODE(U'\U00110000', '\x00', '\x11', '\x00', '\x00');
	TEST_INVALID_CODE_POINT_UTF32BE_DECODE(U'\U7FFFFFFF', '\x7F', '\xFF', '\xFF', '\xFF');
	TEST_INVALID_CODE_POINT_UTF32BE_DECODE(U'\x80000000', '\x80', '\x00', '\x00', '\x00');
	TEST_INVALID_CODE_POINT_UTF32BE_DECODE(U'\xFFFFFFFF', '\xFF', '\xFF', '\xFF', '\xFF');
#undef TEST_INVALID_CODE_POINT_UTF32BE_DECODE
}

TEST_F(UTF32BETest, PolymorphicEncoding) {
	using EncodingBase = ::unicode::Encoding<char, 4>;
	utf32be::PolymorphicEncoder utf32beEncoder;
	EncodingBase::Encoder* encoder = &utf32beEncoder;
	EncodingBase::CodeUnits codeUnits;
	EncodingBase::CodeUnitsCount codeUnitsCount;

	codeUnitsCount = encoder->encode(U'\U0001D11E', codeUnits);
	vector<char> expectedBytes = {'\x00', '\x01', '\xD1', '\x1E'};
	ASSERT_EQ(expectedBytes.size(), codeUnitsCount);
	for(auto i = 0u; i < codeUnitsCount; i++) {
		EXPECT_EQ(expectedBytes[i], codeUnits[i]) << "at index " << i;
	}
}

TEST_F(UTF32BETest, PolymorphicDecoding) {
	utf32be::PolymorphicDecoder utf32beDecoder;
	::unicode::Encoding<char, 4>::Decoder* decoder = &utf32beDecoder;

	EXPECT_FALSE(decoder->partial());

	EXPECT_EQ(utf32be::PartiallyDecoded, decoder->decode('\x00'));
	EXPECT_TRUE(decoder->partial());

	EXPECT_EQ(utf32be::PartiallyDecoded, decoder->decode('\x01'));
	EXPECT_TRUE(decoder->partial());

	EXPECT_EQ(utf32be::PartiallyDecoded, decoder->decode('\xD1'));
	EXPECT_TRUE(decoder->partial());

	EXPECT_EQ(U'\U0001D11E', decoder->decode('\x1E'));
	EXPECT_FALSE(decoder->partial());


	EXPECT_EQ(utf32be::PartiallyDecoded, decoder->decode('\x00'));
	EXPECT_TRUE(decoder->partial());

	EXPECT_EQ(utf32be::PartiallyDecoded, decoder->decode('\x01'));
	EXPECT_TRUE(decoder->partial());

	decoder->reset();
	EXPECT_FALSE(decoder->partial());
}
