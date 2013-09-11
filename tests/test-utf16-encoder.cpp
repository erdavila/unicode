#include "test-utf16.hpp"
using namespace unicode;
using namespace std;

TEST_F(UTF16EncoderTest, InvalidCodePoints) {
	TEST_INVALID_CODE_POINT_ENCODE(U'\U00110000');
	TEST_INVALID_CODE_POINT_ENCODE(U'\U7FFFFFFF');
	TEST_INVALID_CODE_POINT_ENCODE(U'\x80000000');
	TEST_INVALID_CODE_POINT_ENCODE(U'\xFFFFFFFF');
}

TEST_F(UTF16EncoderTest, Polymorphic) {
	using EncodingBase = ::unicode::Encoding<char16_t, 2>;
	utf16::PolymorphicEncoder utf16Encoder;
	EncodingBase::Encoder* encoder = &utf16Encoder;
	EncodingBase::CodeUnits codeUnits;
	EncodingBase::CodeUnitsCount codeUnitsCount;

	codeUnitsCount = encoder->encode(U'@', codeUnits);
	EXPECT_EQ(1u, codeUnitsCount);
	EXPECT_EQ(u'@', codeUnits[0]);

	codeUnitsCount = encoder->encode(U'\U0001D11E', codeUnits);
	vector<char16_t> expectedCodeUnits = {u'\xD834', u'\xDD1E'};
	ASSERT_EQ(expectedCodeUnits.size(), codeUnitsCount);
	for(auto i = 0u; i < codeUnitsCount; i++) {
		EXPECT_EQ(expectedCodeUnits[i], codeUnits[i]) << "at index " << i;
	}
}
