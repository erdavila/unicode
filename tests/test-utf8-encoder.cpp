#include "test-utf8.hpp"
using namespace unicode;
using namespace std;

TEST_F(UTF8EncoderTest, InvalidCodePoints) {
	TEST_INVALID_CODE_POINT_ENCODE(U'\U00110000');
	TEST_INVALID_CODE_POINT_ENCODE(U'\U7FFFFFFF');
	TEST_INVALID_CODE_POINT_ENCODE(U'\x80000000');
	TEST_INVALID_CODE_POINT_ENCODE(U'\xFFFFFFFF');
}

TEST_F(UTF8EncoderTest, Polymorphic) {
	using EncodingBase = ::unicode::Encoding<char, 4>;
	utf8::PolymorphicEncoder utf8Encoder;
	EncodingBase::Encoder* encoder = &utf8Encoder;
	EncodingBase::CodeUnits codeUnits;
	EncodingBase::CodeUnitsCount codeUnitsCount;

	codeUnitsCount = encoder->encode(U'@', codeUnits);
	EXPECT_EQ(1u, codeUnitsCount);
	EXPECT_EQ('@', codeUnits[0]);

	codeUnitsCount = encoder->encode(U'\U0001D11E', codeUnits);
	vector<char> expectedBytes = {'\xF0', '\x9D', '\x84', '\x9E'};
	ASSERT_EQ(expectedBytes.size(), codeUnitsCount);
	for(auto i = 0u; i < codeUnitsCount; i++) {
		EXPECT_EQ(expectedBytes[i], codeUnits[i]) << "at index " << i;
	}
}
