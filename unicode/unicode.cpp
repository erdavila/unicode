#include "unicode.hpp"


namespace unicode {


void utf8::Encoder::encode(char32_t ch, CodeUnits& codeUnits, CodeUnitsCount& codeUnitsCount) {
	if(ch <= 0x7F) {
		codeUnitsCount = 1;
		codeUnits[0] = ch;
	} else {
		codeUnitsCount = 2;
		codeUnits[1] = (ch & 0x3F/*00111111*/) | 0x80/*10000000*/;
		codeUnits[0] = (ch >> 6) | 0xC0/*11000000*/;
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
