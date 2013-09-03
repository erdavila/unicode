#include "unicode.hpp"


namespace unicode {


void utf8::Encoder::encode(char32_t ch, CodeUnits& codeUnits, CodeUnitsCount& codeUnitsCount) {
	if(ch <= 0x7F) {
		codeUnitsCount = 1;
		codeUnits[0] = ch;
	} else if(ch <= 0x07FF) {
		codeUnitsCount = 2;
		codeUnits[1] = (ch & 0x3F/*00111111*/) | 0x80/*10000000*/;
		codeUnits[0] = (ch >> 6) | 0xC0/*11000000*/;
	} else {
		codeUnitsCount = 3;
		for(int i = codeUnitsCount - 1; i > 0; i--) {
			codeUnits[i] = (ch & 0x3F/*00111111*/) | 0x80/*10000000*/;
			ch >>= 6;
		}
		codeUnits[0] = ch | 0xE0/*11100000*/;
	}
}

char32_t utf8::Decoder::decode(CodeUnit) {
	return Partial;
}

void utf32::Encoder::encode(char32_t, CodeUnits&, CodeUnitsCount&) {
	;
}

char32_t utf32::Decoder::decode(CodeUnit) {
	return Partial;
}


}
