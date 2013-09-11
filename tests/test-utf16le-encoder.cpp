#include "test-utf16le.hpp"
using namespace unicode;
using namespace std;

TEST_F(UTF16LEEncoderTest, InvalidCodePoints) {
	TEST_INVALID_CODE_POINT_ENCODE(U'\U00110000');
	TEST_INVALID_CODE_POINT_ENCODE(U'\U7FFFFFFF');
	TEST_INVALID_CODE_POINT_ENCODE(U'\x80000000');
	TEST_INVALID_CODE_POINT_ENCODE(U'\xFFFFFFFF');
}

TEST_F(UTF16LEEncoderTest, Polymorphic) {
	using EncodingBase = ::unicode::Encoding<char, 4>;
	utf16le::PolymorphicEncoder utf16LEEncoder;
	EncodingBase::Encoder* encoder = &utf16LEEncoder;
	EncodingBase::CodeUnits codeUnits;
	EncodingBase::CodeUnitsCount codeUnitsCount;

	codeUnitsCount = encoder->encode(U'@', codeUnits);
	EXPECT_EQ(2u, codeUnitsCount);
	EXPECT_EQ('@', codeUnits[0]);
	EXPECT_EQ('\0', codeUnits[1]);

	codeUnitsCount = encoder->encode(U'\U0001D11E', codeUnits);
	vector<char> expectedCodeUnits = {'\x34', '\xD8', '\x1E', '\xDD'};
	ASSERT_EQ(expectedCodeUnits.size(), codeUnitsCount);
	for(auto i = 0u; i < codeUnitsCount; i++) {
		EXPECT_EQ(expectedCodeUnits[i], codeUnits[i]) << "at index " << i;
	}
}
