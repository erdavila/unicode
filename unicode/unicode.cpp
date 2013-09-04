#include "unicode.hpp"


namespace unicode {


void utf8::Encoder::encode(char32_t ch, CodeUnits& codeUnits, CodeUnitsCount& codeUnitsCount) {
	byte leadingByteMask;
	if(ch <= 0x7F) {
		codeUnitsCount = 1;
		leadingByteMask = 0x00/*0-------*/;
	} else if(ch <= 0x07FF) {
		codeUnitsCount = 2;
		leadingByteMask = 0xC0/*110-----*/;
	} else if(ch <= 0xFFFF) {
		codeUnitsCount = 3;
		leadingByteMask = 0xE0/*1110----*/;
	} else if(ch <= 0x10FFFF) {
		codeUnitsCount = 4;
		leadingByteMask = 0xF0/*11110---*/;
	} else {
		throw InvalidCodePoint(ch);
	}

	for(int i = codeUnitsCount - 1; i > 0; i--) {
		codeUnits[i] = (ch & 0x3F/*00111111*/) | 0x80/*10------*/;
		ch >>= 6;
	}
	codeUnits[0] = ch | leadingByteMask;
}

char32_t utf8::Decoder::decode(CodeUnit codeUnit) {
	return codeUnit;
}

void utf32::Encoder::encode(char32_t, CodeUnits&, CodeUnitsCount&) {
	NOT_IMPLEMENTED
}

char32_t utf32::Decoder::decode(CodeUnit) {
	NOT_IMPLEMENTED
}


}
