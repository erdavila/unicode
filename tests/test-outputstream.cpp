#include "test.hpp"
#include <initializer_list>
#include <sstream>
using namespace unicode;
using namespace std;


template <typename EncodingFrom, typename EncodingTo>
class OutputStreamTestBase : public ::testing::Test {
protected:
	using OStream = basic_ostringstream<char16_t>;
	OStream ostrs;
	OutputStream<EncodingFrom, EncodingTo, OStream> os;

	OutputStreamTestBase() : os(makeOutputStream<utf32le, utf16>(ostrs)) {}

	void testOutputStream(initializer_list<typename EncodingFrom::CodeUnit> inputCodeUnits, initializer_list<typename EncodingTo::CodeUnit> expectedOutputCodeUnits) {
		int index = 0;
		for(typename EncodingFrom::CodeUnit inputCodeUnit : inputCodeUnits) {
			os.put(inputCodeUnit);
			index++;
		}

		ASSERT_EQ(basic_string<typename EncodingTo::CodeUnit>(expectedOutputCodeUnits), ostrs.str());
	}
};

using OutputStreamTest = OutputStreamTestBase<utf32le, utf16>;

TEST_F(OutputStreamTest, Empty) {
	testOutputStream({}, {});
}

TEST_F(OutputStreamTest, OneCodePoint) {
	testOutputStream({'@', '\0', '\0', '\0'}, {u'@'});
}

TEST_F(OutputStreamTest, SeveralCodePoints) {
	testOutputStream(
		{
			'\x00', '\x00', '\x00', '\x00',  // U+0000
			'\x34', '\x6C', '\x00', '\x00',  // U+6C34
			'\xFF', '\xFE', '\x00', '\x00',  // U+FEFF
			'\x00', '\x00', '\x01', '\x00',  // U+010000
			'\x1E', '\xD1', '\x01', '\x00',  // U+01D11E
			'\xFF', '\xFF', '\x10', '\x00',  // U+10FFFF
		},
		{
			u'\x0000',  // U+0000
			u'\u6C34',  // U+6C34
			u'\uFEFF',  // U+FEFF
			u'\xD800', u'\xDC00',  // U+010000
			u'\xD834', u'\xDD1E',  // U+01D11E
			u'\xDBFF', u'\xDFFF',  // U+10FFFF
		}
	);
}

TEST_F(OutputStreamTest, InvalidCodePoint) {
	ASSERT_EQ(u"", ostrs.str());


	os.put('@');
	ASSERT_EQ(u"", ostrs.str());

	os.put('\0');
	ASSERT_EQ(u"", ostrs.str());

	os.put('\0');
	ASSERT_EQ(u"", ostrs.str());

	os.put('\0');
	ASSERT_EQ(u"@", ostrs.str());


	os.put('\x00');
	ASSERT_EQ(u"@", ostrs.str());

	os.put('\x00');
	ASSERT_EQ(u"@", ostrs.str());

	os.put('\x11');
	ASSERT_EQ(u"@", ostrs.str());

	try {
		os.put('\x00');
		FAIL() << "should have thrown";
	} catch(InvalidCodePoint& e) {
		ASSERT_EQ(U'\U00110000', e.codePoint);
	}
	ASSERT_EQ(u"@", ostrs.str());


	os.put('$');
	ASSERT_EQ(u"@", ostrs.str());

	os.put('\0');
	ASSERT_EQ(u"@", ostrs.str());

	os.put('\0');
	ASSERT_EQ(u"@", ostrs.str());

	os.put('\0');
	ASSERT_EQ(u"@$", ostrs.str());
}

TEST_F(OutputStreamTest, IncompleteInput) {
	ASSERT_FALSE(os.incompleteInput());

	os.put('@');
	ASSERT_TRUE(os.incompleteInput());

	os.put('\0');
	ASSERT_TRUE(os.incompleteInput());

	os.put('\0');
	ASSERT_TRUE(os.incompleteInput());

	os.put('\0');
	ASSERT_FALSE(os.incompleteInput());
}
