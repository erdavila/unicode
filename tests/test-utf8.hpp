#ifndef TEST_UTF8_HPP_
#define TEST_UTF8_HPP_

#include "test.hpp"

class UTF8EncoderTest : public EncoderTest< ::unicode::utf8> {
	void simpleEncodeCheck() override {
		testEncode(U'\U0001D11E', {'\xF0', '\x9D', '\x84', '\x9E'});
	}
};

class UTF8DecoderTest : public DecoderTest< ::unicode::utf8> {
protected:
	void simpleDecodeCheck() override {
		testDecode(U'\U0001D11E', {'\xF0', '\x9D', '\x84', '\x9E'});
	}
};

#endif /* TEST_UTF8_HPP_ */
