#ifndef TEST_HPP_
#define TEST_HPP_

#include "unicode/unicode.hpp"
#include <initializer_list>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
#ifdef __CYGWIN__
// Why can't Google Test see the regular declarations of the functions below on Cygwin GCC?!
# include <cstdio>
# include <cstring>
int __attribute__((__cdecl__)) fileno(FILE *);
char *strdup(const char *s);
FILE *fdopen(int fd, const char *mode);
#endif
#include "gtest/gtest.h"


inline std::string to_hex(unsigned long value, size_t width) {
	std::ostringstream ostrs;
	ostrs.width(width);
	ostrs.fill('0');
	ostrs << std::hex << std::uppercase << value;
	return ostrs.str();
}

template <typename Encoding_>
struct EncoderTest : virtual ::testing::Test {
	using Encoding = Encoding_;
	typename Encoding::Encoder encoder;

	void testEncode(char32_t codePoint, const std::vector<typename Encoding::CodeUnit>& expectedCodeUnits) {
		typename Encoding::CodeUnits codeUnits;
		typename Encoding::CodeUnitsCount codeUnitsCount = encoder.encode(codePoint, codeUnits);
		ASSERT_EQ(expectedCodeUnits.size(), codeUnitsCount);
		for(auto i = 0u; i < expectedCodeUnits.size(); i++) {
			EXPECT_EQ(expectedCodeUnits[i], codeUnits[i]) << "at index " << i;
		}
	}

	void testInvalidCodePointEncode(char32_t codePoint) {
		typename Encoding::CodeUnits codeUnits;

		try {
			encoder.encode(codePoint, codeUnits);
			FAIL() << "Exception not thrown";
		} catch(typename Encoding::InvalidCodePoint& e) {
			EXPECT_EQ(codePoint, e.codePoint);
		}

		// Test if the encoder still works after throwing
		simpleEncodeCheck();
	}

	virtual void simpleEncodeCheck() = 0;
};

#define TEST_INVALID_CODE_POINT_ENCODE(codePoint) \
	{ SCOPED_TRACE(#codePoint); testInvalidCodePointEncode(codePoint); }

template <typename Encoding_>
struct DecoderTest : virtual ::testing::Test {
	using Encoding = Encoding_;
	typename Encoding::Decoder decoder;
	enum : char32_t { NO_CODE_POINT = 0xFFFFFFFF };

	void testDecode(char32_t expectedCodePoint, const std::vector<typename Encoding::CodeUnit>& codeUnits) {
		const auto lastIndex = codeUnits.size() - 1;
		for(auto i = 0u; i <= lastIndex ; i++) {
			typename Encoding::CodeUnit codeUnit = codeUnits[i];
			char32_t codePoint = decoder.decode(codeUnit);

			if(i == lastIndex) {
				EXPECT_EQ(expectedCodePoint, codePoint) << "U+" << to_hex(codePoint, 4) << " instead of U+" << to_hex(expectedCodePoint, 4);
			} else {
				ASSERT_EQ(Encoding::PartiallyDecoded, codePoint) << "at index " << i;
			}
		}
	}

	template <typename Exception>
	void testDecodeWithFailure(char32_t codePoint, const std::vector<typename Encoding::CodeUnit>& codeUnits) {
		auto lastIndex = codeUnits.size() - 1;
		for(auto i = 0u; i < lastIndex; i++) {
			typename Encoding::CodeUnit codeUnit = codeUnits[i];
			char32_t codePoint = decoder.decode(codeUnit);
			ASSERT_EQ(Encoding::PartiallyDecoded, codePoint) << "at index " << i;
		}

		try {
			typename Encoding::CodeUnit codeUnit = codeUnits[lastIndex];
			char32_t codePoint = decoder.decode(codeUnit);
			FAIL() << "Returned U+" << to_hex(codePoint, 4) << " instead of throwing";
		} catch(Exception& e) {
			try {
				::unicode::CodePointException& cpe = dynamic_cast< ::unicode::CodePointException&>(e);
				EXPECT_EQ(codePoint, cpe.codePoint) << "Exception code point is U+" << to_hex(cpe.codePoint, 4);
			} catch(std::bad_cast& bd) {
				EXPECT_EQ(NO_CODE_POINT, codePoint) << "The exception thrown does not include a code point";
			}
		}

		// Test if the decoder still works after throwing
		simpleDecodeCheck();
	}

	virtual void simpleDecodeCheck() = 0;
};

#define TEST_DECODE_WITH_FAILURE(exception, codePoint, codeUnits...) \
		{ SCOPED_TRACE(#codePoint); testDecodeWithFailure<exception>(codePoint, {codeUnits}); }

#define TEST_INVALID_CODE_POINT_DECODE(codePoint, codeUnits...) \
	TEST_DECODE_WITH_FAILURE(Encoding::InvalidCodePoint, codePoint, codeUnits)

#define SIMPLE_DECODE_CHECK() { SCOPED_TRACE(""); simpleDecodeCheck(); }


template <typename EncoderTest, typename DecoderTest>
struct EncodingTest : EncoderTest, DecoderTest {
	static_assert(std::is_same<typename EncoderTest::Encoding, typename DecoderTest::Encoding>::value,
			"EncoderTest and DecoderTest should test the same encoding");
	using Encoding = typename EncoderTest::Encoding;

	void testCodePointEncoding(char32_t codePoint, const std::vector<typename Encoding::CodeUnit>& codeUnits) {
		this->testEncode(codePoint, codeUnits);
		this->testDecode(codePoint, codeUnits);
	}
};

#define TEST_CODE_POINT_ENCODING(codePoint, codeUnits...) \
	{ SCOPED_TRACE(#codePoint); testCodePointEncoding(codePoint, {codeUnits}); }


#endif /* TEST_HPP_ */
