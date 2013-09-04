#include "unicode.hpp"


namespace unicode {


namespace /*unnamed*/ {
	namespace utf8_impl {
		enum DecodeState {
			BEGIN, LEADING2, LEADING3, LEADING4
		};

		enum class ByteType {
			ASCII, CONTINUATION,
			LEADING2, LEADING3, LEADING4
		};

		ByteType byteType(utf8::CodeUnit codeUnit) {
			if(codeUnit >> 7 == 0x00/*0-------*/) {
				return ByteType::ASCII;
			} else if(codeUnit >> 6 == 0x02/*10------*/) {
				return ByteType::CONTINUATION;
			} else if(codeUnit >> 5 == 0x06/*110-----*/) {
				return ByteType::LEADING2;
			} else if(codeUnit >> 4 == 0x0E/*1110----*/) {
				return ByteType::LEADING3;
			} else if(codeUnit >> 3 == 0x1E/*11110---*/) {
				return ByteType::LEADING4;
			} else {
				NOT_IMPLEMENTED
			}
		}
	}
}

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
	using namespace utf8_impl;
	ByteType type = byteType(codeUnit);
	char32_t codePoint = PartiallyDecoded;

	switch(state) {
	case BEGIN:
		if(type == ByteType::ASCII) {
			return codeUnit;
		} else if(type == ByteType::LEADING2) {
			decoding = codeUnit & 0x1F/*110-----*/;
			pending = 1;
			state = LEADING2;
		} else if(type == ByteType::LEADING3) {
			decoding = codeUnit & 0x0F/*1110----*/;
			pending = 2;
			state = LEADING3;
		} else if(type == ByteType::LEADING4) {
			decoding = codeUnit & 0x07/*11110---*/;
			pending = 3;
			state = LEADING4;
		} else {
			NOT_IMPLEMENTED;
		}
		break;
	case LEADING2:
	case LEADING3:
	case LEADING4:
		if(type == ByteType::CONTINUATION) {
			decoding = (decoding << 6) | (codeUnit & 0x3F/*10------*/);
			if(--pending == 0) {
				state = BEGIN;
				codePoint = decoding;
			}
		} else {
			NOT_IMPLEMENTED
		}
		break;
	default:
		NOT_IMPLEMENTED;
		break;
	}

	return codePoint;
}


void utf32::Encoder::encode(char32_t, CodeUnits&, CodeUnitsCount&) {
	NOT_IMPLEMENTED
}

char32_t utf32::Decoder::decode(CodeUnit) {
	NOT_IMPLEMENTED
}


}
