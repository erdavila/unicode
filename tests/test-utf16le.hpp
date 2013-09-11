#ifndef TEST_UTF16LE_HPP_
#define TEST_UTF16LE_HPP_

#include "test.hpp"

class UTF16LEEncoderTest : public EncoderTest< ::unicode::utf16le> {
protected:
	void simpleEncodeCheck() override {
		testEncode(U'\U0001D11E', {'\x34', '\xD8', '\x1E', '\xDD'});
	}
};

class UTF16LEDecoderTest : public DecoderTest< ::unicode::utf16le> {
protected:
	void simpleDecodeCheck() override {
		testDecode(U'\U0001D11E', {'\x34', '\xD8', '\x1E', '\xDD'});
	}
};


#endif /* TEST_UTF16LE_HPP_ */
