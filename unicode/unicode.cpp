#include "unicode.hpp"


namespace unicode {


namespace /*unnamed*/ {
	namespace utf8_impl {
		enum DecodeState {
			NEUTRAL, TWO_BYTES, THREE_BYTES, FOUR_BYTES
		};

		char32_t minCodePoint(int state) {
			switch(state) {
			default:
			case NEUTRAL:     return U'\u0000';
			case TWO_BYTES:   return U'\u0080';
			case THREE_BYTES: return U'\u0800';
			case FOUR_BYTES:  return U'\U00010000';
			}
		}
	}
}


CodePointException::CodePointException(const char* problem, char32_t codePoint)
	: Exception(msg(problem, codePoint)), codePoint(codePoint)
	{}

std::string CodePointException::msg(const char* problem, char32_t codePoint) {
	std::ostringstream ostrs;
	ostrs << problem << " U+";
	ostrs.fill('0');
	ostrs.width(4);
	ostrs << std::hex << codePoint;
	return ostrs.str();
}


CodeUnitsCount utf8::Encoder::encode(char32_t ch, CodeUnits& codeUnits) {
	byte leadingByteMask;
	CodeUnitsCount codeUnitsCount;
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

	return codeUnitsCount;
}

char32_t utf8::Decoder::decode(CodeUnit codeUnit) {
	using namespace utf8_impl;

	ByteType type = byteType(codeUnit);
	if(type == ByteType::INVALID) {
		state = NEUTRAL;
		throw InvalidByte();
	}

	char32_t codePoint = PartiallyDecoded;
	if(state == NEUTRAL) {
		if(type == ByteType::ASCII) {
			codePoint = codeUnit & 0x7F/*0-------*/;
		} else if(type == ByteType::LEADING2) {
			decoding = codeUnit & 0x1F/*110-----*/;
			pending = 1;
			state = TWO_BYTES;
		} else if(type == ByteType::LEADING3) {
			decoding = codeUnit & 0x0F/*1110----*/;
			pending = 2;
			state = THREE_BYTES;
		} else if(type == ByteType::LEADING4) {
			decoding = codeUnit & 0x07/*11110---*/;
			pending = 3;
			state = FOUR_BYTES;
		} else if(type == ByteType::CONTINUATION) {
			throw UnexpectedContinuationByte();
		} else {
			NOT_IMPLEMENTED;
		}
	} else {
		if(type == ByteType::CONTINUATION) {
			decoding = (decoding << 6) | (codeUnit & 0x3F/*10------*/);
			if(--pending == 0) {
				codePoint = decoding;
				auto prevState = state;
				state = NEUTRAL;
				if(codePoint > 0x10FFFF) {
					throw InvalidCodePoint(codePoint);
				}
				if(codePoint < minCodePoint(prevState)) {
					throw OverlongEncoding(codePoint);
				}
			}
		} else {
			NOT_IMPLEMENTED
		}
	}

	return codePoint;
}

auto utf8::byteType(CodeUnit codeUnit) -> ByteType {
	if(codeUnit >> 7 == 0x00/*0-------*/) {
		return ByteType::ASCII;
	} else if(codeUnit >> 6 == 0x02/*10------*/) {
		return ByteType::CONTINUATION;
	} else if(codeUnit >> 5 == 0x06/*110-----*/) {
		return ByteType::LEADING2;
	} else if(codeUnit >> 4 == 0x0E/*1110----*/) {
		return ByteType::LEADING3;
	} else if(codeUnit >> 3 == 0x1E/*11110---*/  &&  codeUnit <= 0xF4/*11110100*/) {
		return ByteType::LEADING4;
	} else {
		return ByteType::INVALID;
	}
}


CodeUnitsCount utf32::Encoder::encode(char32_t, CodeUnits&) {
	NOT_IMPLEMENTED
}

char32_t utf32::Decoder::decode(CodeUnit) {
	NOT_IMPLEMENTED
}


}
