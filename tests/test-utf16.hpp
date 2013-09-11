#ifndef TEST_UTF16_HPP_
#define TEST_UTF16_HPP_

#include "test.hpp"

class UTF16EncoderTest : public EncoderTest< ::unicode::utf16> {
	void simpleEncodeCheck() override {
		testEncode(U'\U0001D11E', {u'\xD834', u'\xDD1E'});
	}
};

class UTF16DecoderTest : public DecoderTest< ::unicode::utf16> {
protected:
	void simpleDecodeCheck() override {
		testDecode(U'\U0001D11E', {u'\xD834', u'\xDD1E'});
	}
};

#endif /* TEST_UTF16_HPP_ */
