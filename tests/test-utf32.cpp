#include "test.hpp"
using namespace unicode;
using namespace std;

class UTF32EncoderTest : public EncoderTest<utf32> {
protected:
	void simpleEncodeCheck() override {
		testEncode(U'\U0001D11E', {U'\U0001D11E'});
	}
};

class UTF32DecoderTest : public DecoderTest<utf32> {
protected:
	void simpleDecodeCheck() override {
		testDecode(U'\U0001D11E', {U'\U0001D11E'});
	}
};

class UTF32Test : public EncodingTest<UTF32EncoderTest, UTF32DecoderTest> {};

TEST_F(UTF32Test, Encoding) {
#define TEST_CODE_POINT_UTF32_ENCODING(codePoint) TEST_CODE_POINT_ENCODING(codePoint, codePoint)
	TEST_CODE_POINT_UTF32_ENCODING(U'\x0000');
	TEST_CODE_POINT_UTF32_ENCODING(U'\u0020');
	TEST_CODE_POINT_UTF32_ENCODING(U'\u0024');
	TEST_CODE_POINT_UTF32_ENCODING(U'\u007F');
	TEST_CODE_POINT_UTF32_ENCODING(U'\u0080');
	TEST_CODE_POINT_UTF32_ENCODING(U'\u00A2');
	TEST_CODE_POINT_UTF32_ENCODING(U'\u02AA');
	TEST_CODE_POINT_UTF32_ENCODING(U'\u07FF');
	TEST_CODE_POINT_UTF32_ENCODING(U'\u0800');
	TEST_CODE_POINT_UTF32_ENCODING(U'\u20AC');
	TEST_CODE_POINT_UTF32_ENCODING(U'\uFEFF');
	TEST_CODE_POINT_UTF32_ENCODING(U'\uFFFF');
	TEST_CODE_POINT_UTF32_ENCODING(U'\U00010000');
	TEST_CODE_POINT_UTF32_ENCODING(U'\U0001D11E');
	TEST_CODE_POINT_UTF32_ENCODING(U'\U00024B62');
	TEST_CODE_POINT_UTF32_ENCODING(U'\U0010FFFF');
#undef TEST_CODE_POINT_UTF32_ENCODING
}

TEST_F(UTF32Test, InvalidCodePoints) {
	TEST_INVALID_CODE_POINT_ENCODE(U'\U00110000');
	TEST_INVALID_CODE_POINT_ENCODE(U'\U7FFFFFFF');
	TEST_INVALID_CODE_POINT_ENCODE(U'\x80000000');
	TEST_INVALID_CODE_POINT_ENCODE(U'\xFFFFFFFF');

#define TEST_INVALID_CODE_POINT_UTF32_DECODE(codePoint) TEST_INVALID_CODE_POINT_DECODE(codePoint, codePoint)
	TEST_INVALID_CODE_POINT_UTF32_DECODE(U'\U00110000');
	TEST_INVALID_CODE_POINT_UTF32_DECODE(U'\U7FFFFFFF');
	TEST_INVALID_CODE_POINT_UTF32_DECODE(U'\x80000000');
	TEST_INVALID_CODE_POINT_UTF32_DECODE(U'\xFFFFFFFF');
#undef TEST_INVALID_CODE_POINT_UTF32_DECODE
}

TEST_F(UTF32Test, Polymorphic) {
	using EncodingBase = ::unicode::Encoding<char32_t, 1>;

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
