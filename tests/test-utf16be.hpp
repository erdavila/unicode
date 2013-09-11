#ifndef TEST_UTF16BE_HPP_
#define TEST_UTF16BE_HPP_

#include "test.hpp"

class UTF16BEEncoderTest : public EncoderTest< ::unicode::utf16be> {
protected:
	void simpleEncodeCheck() override {
		testEncode(U'\U0001D11E', {'\xD8', '\x34', '\xDD', '\x1E'});
	}
};

class UTF16BEDecoderTest : public DecoderTest< ::unicode::utf16be> {
protected:
	void simpleDecodeCheck() override {
		testDecode(U'\U0001D11E', {'\xD8', '\x34', '\xDD', '\x1E'});
	}
};


#endif /* TEST_UTF16BE_HPP_ */
