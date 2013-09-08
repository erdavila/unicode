#include "test.hpp"
using namespace unicode;
using namespace std;

class UTF32LEEncoderTest : public EncoderTest<utf32le> {
protected:
	void simpleEncodeCheck() override {
		testEncode(U'\U0001D11E', {'\x1E', '\xD1', '\x01', '\x00'});
	}
};

class UTF32LEDecoderTest : public DecoderTest<utf32le> {
protected:
	void simpleDecodeCheck() override {
		testDecode(U'\U0001D11E', {'\x1E', '\xD1', '\x01', '\x00'});
	}
};

class UTF32LETest : public EncodingTest<UTF32LEEncoderTest, UTF32LEDecoderTest> {};

TEST_F(UTF32LETest, Encoding) {
	TEST_CODE_POINT_ENCODING(U'\x0000', '\x00', '\x00', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\u0020', '\x20', '\x00', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\u0024', '\x24', '\x00', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\u007F', '\x7F', '\x00', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\u0080', '\x80', '\x00', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\u00A2', '\xA2', '\x00', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\u02AA', '\xAA', '\x02', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\u07FF', '\xFF', '\x07', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\u0800', '\x00', '\x08', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\u20AC', '\xAC', '\x20', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\uFEFF', '\xFF', '\xFE', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\uFFFF', '\xFF', '\xFF', '\x00', '\x00');
	TEST_CODE_POINT_ENCODING(U'\U00010000', '\x00', '\x00', '\x01', '\x00');
	TEST_CODE_POINT_ENCODING(U'\U0001D11E', '\x1E', '\xD1', '\x01', '\x00');
	TEST_CODE_POINT_ENCODING(U'\U00024B62', '\x62', '\x4B', '\x02', '\x00');
	TEST_CODE_POINT_ENCODING(U'\U0010FFFF', '\xFF', '\xFF', '\x10', '\x00');
}

TEST_F(UTF32LETest, InvalidCodePoints) {
	TEST_INVALID_CODE_POINT_ENCODE(U'\U00110000');
	TEST_INVALID_CODE_POINT_ENCODE(U'\U7FFFFFFF');
	TEST_INVALID_CODE_POINT_ENCODE(U'\x80000000');
	TEST_INVALID_CODE_POINT_ENCODE(U'\xFFFFFFFF');

#define TEST_INVALID_CODE_POINT_UTF32LE_DECODE(codePoint, codeUnits...) TEST_INVALID_CODE_POINT_DECODE(codePoint, codeUnits)
	TEST_INVALID_CODE_POINT_UTF32LE_DECODE(U'\U00110000', '\x00', '\x00', '\x11', '\x00');
	TEST_INVALID_CODE_POINT_UTF32LE_DECODE(U'\U7FFFFFFF', '\xFF', '\xFF', '\xFF', '\x7F');
	TEST_INVALID_CODE_POINT_UTF32LE_DECODE(U'\x80000000', '\x00', '\x00', '\x00', '\x80');
	TEST_INVALID_CODE_POINT_UTF32LE_DECODE(U'\xFFFFFFFF', '\xFF', '\xFF', '\xFF', '\xFF');
#undef TEST_INVALID_CODE_POINT_UTF32LE_DECODE
}

TEST_F(UTF32LETest, PolymorphicEncoding) {
	using EncodingBase = ::unicode::Encoding<char, 4>;
	utf32le::PolymorphicEncoder utf32leEncoder;
	EncodingBase::Encoder* encoder = &utf32leEncoder;
	EncodingBase::CodeUnits codeUnits;
	EncodingBase::CodeUnitsCount codeUnitsCount;

	codeUnitsCount = encoder->encode(U'\U0001D11E', codeUnits);
	vector<char> expectedBytes = {'\x1E', '\xD1', '\x01', '\x00'};
	ASSERT_EQ(expectedBytes.size(), codeUnitsCount);
	for(auto i = 0u; i < codeUnitsCount; i++) {
		EXPECT_EQ(expectedBytes[i], codeUnits[i]) << "at index " << i;
	}
}

TEST_F(UTF32LETest, PolymorphicDecoding) {
	utf32le::PolymorphicDecoder utf32leDecoder;
	::unicode::Encoding<char, 4>::Decoder* decoder = &utf32leDecoder;

	EXPECT_FALSE(decoder->partial());

	EXPECT_EQ(utf32le::PartiallyDecoded, decoder->decode('\x1E'));
	EXPECT_TRUE(decoder->partial());

	EXPECT_EQ(utf32le::PartiallyDecoded, decoder->decode('\xD1'));
	EXPECT_TRUE(decoder->partial());

	EXPECT_EQ(utf32le::PartiallyDecoded, decoder->decode('\x01'));
	EXPECT_TRUE(decoder->partial());

	EXPECT_EQ(U'\U0001D11E', decoder->decode('\x00'));
	EXPECT_FALSE(decoder->partial());


	EXPECT_EQ(utf32le::PartiallyDecoded, decoder->decode('\x00'));
	EXPECT_TRUE(decoder->partial());

	EXPECT_EQ(utf32le::PartiallyDecoded, decoder->decode('\x01'));
	EXPECT_TRUE(decoder->partial());

	decoder->reset();
	EXPECT_FALSE(decoder->partial());
}
