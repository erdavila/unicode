#include "unicode.hpp"


namespace unicode {


namespace /*unnamed*/ {
	namespace utf8_impl {

		enum DecodeState {
			NEUTRAL, TWO_BYTES, THREE_BYTES, FOUR_BYTES
		};

		template <unsigned int N>
		struct EncodingByte {
			enum { CODE_UNIT_BITS = N };
			enum : char {
				MASK   = char(~((~0) << N)),             // 8-N bits "0" followed by N bits "1"
				MARKER = char(0xFF & ((~0) << (N + 1))), // 8-N-1 bits "1" followed by N+1 bits "0"
			};
			static bool matches(char b) {
				b &= ~MASK;
				return b == MARKER;
			}
			static char32_t decode(char codeUnit) {
				return codeUnit & MASK;
			}
		};

		template <char32_t MinCodePoint, char32_t MaxCodePoint>
		struct EncodingByNumberOfBytes {
			enum {
				MIN_CODE_POINT = MinCodePoint,
				MAX_CODE_POINT = MaxCodePoint,
			};
		};

		struct OneByte : EncodingByNumberOfBytes<U'\u0000', U'\u007F'> {
			// 0-------
			using Byte = EncodingByte<7>;
		};

		// 10------
		struct Continuation : EncodingByte<6> {
			static char extractAndEncode(char32_t& codePoint) {
				char continuationByte = Continuation::MARKER
				        |  (codePoint & Continuation::MASK);
				codePoint >>= Continuation::CODE_UNIT_BITS;
				return continuationByte;
			}
			static void decodeAndInsert(char codeUnit, char32_t& codePoint) {
				codePoint <<= Continuation::CODE_UNIT_BITS;
				codePoint |= Continuation::decode(codeUnit);
			}
		};

		struct TwoBytes : EncodingByNumberOfBytes<U'\u0080', U'\u07FF'> {
			// 110-----
			using Leading = EncodingByte<5>;
		};

		struct ThreeBytes : EncodingByNumberOfBytes<U'\u0800', U'\uFFFF'> {
			// 1110----
			using Leading = EncodingByte<4>;
		};

		struct FourBytes : EncodingByNumberOfBytes<U'\U00010000', MAX_CODE_POINT> {
			// 11110---
			struct Leading : EncodingByte<3> {
				enum { MAX_VALUE = 0xF4/*11110100*/ };
			};
		};

		char32_t minCodePoint(int state) {
			switch(state) {
			default:
			case NEUTRAL:     return    OneByte::MIN_CODE_POINT;
			case TWO_BYTES:   return   TwoBytes::MIN_CODE_POINT;
			case THREE_BYTES: return ThreeBytes::MIN_CODE_POINT;
			case FOUR_BYTES:  return  FourBytes::MIN_CODE_POINT;
			}
		}
	}
}


CodePointException::CodePointException(const char* problem, char32_t codePoint) noexcept
	: Exception(msg(problem, codePoint)), codePoint(codePoint)
	{}

std::string CodePointException::msg(const char* problem, char32_t codePoint) noexcept {
	std::ostringstream ostrs;
	ostrs << problem << " U+";
	ostrs.fill('0');
	ostrs.width(4);
	ostrs << std::hex << codePoint;
	return ostrs.str();
}


CodeUnitsCount utf8::Encoder::encode(char32_t codePoint, CodeUnits& codeUnits) {
	using namespace utf8_impl;

	char firstByteMarker;
	CodeUnitsCount codeUnitsCount;
	if(codePoint <= OneByte::MAX_CODE_POINT) {
		codeUnitsCount = 1;
		firstByteMarker = OneByte::Byte::MARKER;
	} else if(codePoint <= TwoBytes::MAX_CODE_POINT) {
		codeUnitsCount = 2;
		firstByteMarker = TwoBytes::Leading::MARKER;
	} else if(codePoint <= ThreeBytes::MAX_CODE_POINT) {
		codeUnitsCount = 3;
		firstByteMarker = ThreeBytes::Leading::MARKER;
	} else if(codePoint <= FourBytes::MAX_CODE_POINT) {
		codeUnitsCount = 4;
		firstByteMarker = FourBytes::Leading::MARKER;
	} else {
		throw InvalidCodePoint(codePoint);
	}

	for(int i = codeUnitsCount - 1; i > 0; i--) {
		codeUnits[i] = Continuation::extractAndEncode(codePoint);
	}
	codeUnits[0] = codePoint | firstByteMarker;

	return codeUnitsCount;
}

char32_t utf8::Decoder::decode(CodeUnit codeUnit) {
	using namespace utf8_impl;

	ByteType codeUnitType = byteType(codeUnit);
	if(codeUnitType == ByteType::INVALID) {
		state = NEUTRAL;
		throw InvalidByte();
	}

	char32_t codePoint = PartiallyDecoded;
	if(state == NEUTRAL) {
		if(codeUnitType == ByteType::ASCII) {
			codePoint = OneByte::Byte::decode(codeUnit);
		} else if(codeUnitType == ByteType::LEADING2) {
			decoding = TwoBytes::Leading::decode(codeUnit);
			pending = 1;
			state = TWO_BYTES;
		} else if(codeUnitType == ByteType::LEADING3) {
			decoding = ThreeBytes::Leading::decode(codeUnit);
			pending = 2;
			state = THREE_BYTES;
		} else if(codeUnitType == ByteType::LEADING4) {
			decoding = FourBytes::Leading::decode(codeUnit);
			pending = 3;
			state = FOUR_BYTES;
		} else {
			assert(codeUnitType == ByteType::CONTINUATION);
			throw UnexpectedContinuationByte();
		}
	} else {
		if(codeUnitType == ByteType::CONTINUATION) {
			Continuation::decodeAndInsert(codeUnit, decoding);
			if(--pending == 0) {
				codePoint = decoding;
				auto prevState = state;
				state = NEUTRAL;
				if(codePoint > MAX_CODE_POINT) {
					throw InvalidCodePoint(codePoint);
				}
				if(codePoint < minCodePoint(prevState)) {
					throw OverlongEncoding(codePoint);
				}
			}
		} else {
			state = NEUTRAL;
			throw ExpectedContinuationByte();
		}
	}

	return codePoint;
}

bool utf8::Decoder::partial() const noexcept {
	return state != utf8_impl::NEUTRAL;
}

void utf8::Decoder::reset() noexcept {
	state = utf8_impl::NEUTRAL;
}

auto utf8::byteType(char b) noexcept -> ByteType {
	using namespace utf8_impl;

	if(OneByte::Byte::matches(b)) {
		return ByteType::ASCII;
	} else if(Continuation::matches(b)) {
		return ByteType::CONTINUATION;
	} else if(TwoBytes::Leading::matches(b)) {
		return ByteType::LEADING2;
	} else if(ThreeBytes::Leading::matches(b)) {
		return ByteType::LEADING3;
	} else if(FourBytes::Leading::matches(b)  &&  (unsigned char)(b) <= FourBytes::Leading::MAX_VALUE) {
		return ByteType::LEADING4;
	} else {
		return ByteType::INVALID;
	}
}


CodeUnitsCount utf32::Encoder::encode(char32_t codePoint, CodeUnits& codeUnits) {
	if(codePoint > MAX_CODE_POINT) {
		throw InvalidCodePoint(codePoint);
	}
	codeUnits[0] = codePoint;
	return 1;
}

char32_t utf32::Decoder::decode(CodeUnit codeUnit) {
	char32_t codePoint = codeUnit;
	if(codePoint > MAX_CODE_POINT) {
		throw InvalidCodePoint(codePoint);
	}
	return codePoint;
}

bool utf32::Decoder::partial() const noexcept { return false; }

void utf32::Decoder::reset() noexcept {}


CodeUnitsCount utf32be::Encoder::encode(char32_t codePoint, CodeUnits& codeUnits) {
	if(codePoint > MAX_CODE_POINT) {
		throw InvalidCodePoint(codePoint);
	}
	for(int i = 0; i < 4; i++) {
		codeUnits[3-i] = codePoint & 0xFF;
		codePoint >>= 8;
	}
	return 4;
}

char32_t utf32be::Decoder::decode(CodeUnit codeUnit) {
	char32_t codePoint = PartiallyDecoded;
	decoding = (decoding << 8) | (codeUnit & 0xFF);
	if(++count == 4) {
		codePoint = decoding;
		decoding = 0;
		count = 0;
		if(codePoint > MAX_CODE_POINT) {
			throw InvalidCodePoint(codePoint);
		}
	}
	return codePoint;
}

bool utf32be::Decoder::partial() const noexcept {
	return count != 0;
}

void utf32be::Decoder::reset() noexcept {
	count = 0;
}


}
