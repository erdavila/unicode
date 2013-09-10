#include "test.hpp"
#include <initializer_list>
#include <sstream>
using namespace unicode;
using namespace std;

template <typename EncodingFrom, typename EncodingTo>
void testInputStream(initializer_list<typename EncodingFrom::CodeUnit> inputCodeUnits, initializer_list<typename EncodingTo::CodeUnit> expectedOutputCodeUnits) {
	basic_istringstream<typename EncodingFrom::CodeUnit> istr(inputCodeUnits);

	auto is = makeInputStream<EncodingFrom, EncodingTo>(istr);
	ASSERT_FALSE(is.eof());

	using IS = decltype(is);
	typename IS::CodeUnitOrEof codeUnit;

	int index = 0;
	for(typename EncodingTo::CodeUnit expectedOutputCodeUnit : expectedOutputCodeUnits) {
		codeUnit = is.get();
		ASSERT_NE(IS::Eof, codeUnit) << "At output index " << index;
		ASSERT_EQ(expectedOutputCodeUnit, codeUnit) << "At output index " << index;
		ASSERT_FALSE(is.eof()) << "At output index " << index;
		index++;
	}

	codeUnit = is.get();
	ASSERT_EQ(IS::Eof, codeUnit);
	ASSERT_TRUE(is.eof());
}


TEST(InputStreamFromUTF8ToUTF32BETest, Empty) {
	testInputStream<utf8, utf32be>({}, {});
}

TEST(InputStreamFromUTF8ToUTF32BETest, OneCodePoint) {
	testInputStream<utf8, utf32be>({'@'}, {'\0', '\0', '\0', '@'});
}

TEST(InputStreamFromUTF8ToUTF32BETest, SeveralCodePoints) {
	testInputStream<utf8, utf32be>(
		{
			'\x24',  // U+0024
			'\xC2', '\xA2',  // U+00A2
			'\xE2', '\x82', '\xAC',  // U+20AC
			'\xF0', '\xA4', '\xAD', '\xA2',  // U+24B62
		},
		{
			'\x00', '\x00', '\x00', '\x24',  // U+0024
			'\x00', '\x00', '\x00', '\xA2',  // U+00A2
			'\x00', '\x00', '\x20', '\xAC',  // U+20AC
			'\x00', '\x02', '\x4B', '\x62',  // U+24B62
		}
	);
}

TEST(InputStreamFromUTF8ToUTF32BETest, InvalidCodePoint) {
	istringstream istr("@\xF4\x90\x80\x80$");

	auto is = makeInputStream<utf8, utf32be>(istr);
	ASSERT_FALSE(is.eof());

	using IS = decltype(is);
	typename IS::CodeUnitOrEof codeUnit;

	codeUnit = is.get();
	ASSERT_NE(IS::Eof, codeUnit);
	ASSERT_EQ('\0', codeUnit);
	ASSERT_FALSE(is.eof());

	codeUnit = is.get();
	ASSERT_NE(IS::Eof, codeUnit);
	ASSERT_EQ('\0', codeUnit);
	ASSERT_FALSE(is.eof());

	codeUnit = is.get();
	ASSERT_NE(IS::Eof, codeUnit);
	ASSERT_EQ('\0', codeUnit);
	ASSERT_FALSE(is.eof());

	codeUnit = is.get();
	ASSERT_NE(IS::Eof, codeUnit);
	ASSERT_EQ('@', codeUnit);
	ASSERT_FALSE(is.eof());

	try {
		is.get();
		FAIL() << "should have thrown";
	} catch(InvalidCodePoint& e) {
		ASSERT_EQ(U'\U00110000', e.codePoint);
	}

	codeUnit = is.get();
	ASSERT_NE(IS::Eof, codeUnit);
	ASSERT_EQ('\0', codeUnit);
	ASSERT_FALSE(is.eof());

	codeUnit = is.get();
	ASSERT_NE(IS::Eof, codeUnit);
	ASSERT_EQ('\0', codeUnit);
	ASSERT_FALSE(is.eof());

	codeUnit = is.get();
	ASSERT_NE(IS::Eof, codeUnit);
	ASSERT_EQ('\0', codeUnit);
	ASSERT_FALSE(is.eof());

	codeUnit = is.get();
	ASSERT_NE(IS::Eof, codeUnit);
	ASSERT_EQ('$', codeUnit);
	ASSERT_FALSE(is.eof());

	codeUnit = is.get();
	ASSERT_EQ(IS::Eof, codeUnit);
	ASSERT_TRUE(is.eof());
}

TEST(InputStreamFromUTF8ToUTF32BETest, IncompleteInput) {
	istringstream istr("@\xE2\x82");

	auto is = makeInputStream<utf8, utf32be>(istr);
	ASSERT_FALSE(is.eof());

	using IS = decltype(is);
	typename IS::CodeUnitOrEof codeUnit;

	codeUnit = is.get();
	ASSERT_NE(IS::Eof, codeUnit);
	ASSERT_EQ('\0', codeUnit);
	ASSERT_FALSE(is.eof());

	codeUnit = is.get();
	ASSERT_NE(IS::Eof, codeUnit);
	ASSERT_EQ('\0', codeUnit);
	ASSERT_FALSE(is.eof());

	codeUnit = is.get();
	ASSERT_NE(IS::Eof, codeUnit);
	ASSERT_EQ('\0', codeUnit);
	ASSERT_FALSE(is.eof());

	codeUnit = is.get();
	ASSERT_NE(IS::Eof, codeUnit);
	ASSERT_EQ('@', codeUnit);
	ASSERT_FALSE(is.eof());

	try {
		is.get();
		FAIL() << "should have thrown";
	} catch(IncompleteInput&) {
		;
	}

	codeUnit = is.get();
	ASSERT_EQ(IS::Eof, codeUnit);
	ASSERT_TRUE(is.eof());
}


TEST(InputStreamFromUTF32ToUTF8, Empty) {
	testInputStream<utf32, utf8>({}, {});
}

TEST(InputStreamFromUTF32ToUTF8, OneCodePoint) {
	testInputStream<utf32, utf8>({U'@'}, {'@'});
}

TEST(InputStreamFromUTF32ToUTF8, SeveralCodePoints) {
	testInputStream<utf32, utf8>(
		{
			U'\u0024',
			U'\u00A2',
			U'\u20AC',
			U'\U00024B62',
		},
		{
			'\x24',  // U+0024
			'\xC2', '\xA2',  // U+00A2
			'\xE2', '\x82', '\xAC',  // U+20AC
			'\xF0', '\xA4', '\xAD', '\xA2',  // U+24B62
		}
	);
}

TEST(InputStreamFromUTF32ToUTF8, InvalidCodePoint) {
	basic_istringstream<char32_t> istr(U"@\U00110000$");

	auto is = makeInputStream<utf32, utf8>(istr);
	ASSERT_FALSE(is.eof());

	using IS = decltype(is);
	typename IS::CodeUnitOrEof codeUnit;

	codeUnit = is.get();
	ASSERT_NE(IS::Eof, codeUnit);
	ASSERT_EQ('@', codeUnit);
	ASSERT_FALSE(is.eof());

	try {
		is.get();
		FAIL() << "should have thrown";
	} catch(InvalidCodePoint& e) {
		ASSERT_EQ(U'\U00110000', e.codePoint);
	}

	codeUnit = is.get();
	ASSERT_NE(IS::Eof, codeUnit);
	ASSERT_EQ('$', codeUnit);
	ASSERT_FALSE(is.eof());

	codeUnit = is.get();
	ASSERT_EQ(IS::Eof, codeUnit);
	ASSERT_TRUE(is.eof());
}
