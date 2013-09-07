#include "unicode.hpp"


namespace unicode {


namespace /*unnamed*/ {
	namespace utf8_impl {

		enum DecodeState {
			NEUTRAL, TWO_BYTES, THREE_BYTES, FOUR_BYTES
		};

		template <unsigned int N>
		struct EncodingByte {
			enum {
				CODE_UNIT_BITS = N,
				MASK = ~((~0) << N),               // 8-N bits "0" followed by N bits "1"
				MARKER = 0xFF & ((~0) << (N + 1)), // 8-N-1 bits "1" followed by N+1 bits "0"
			};
			static bool matches(byte b) {
				b &= ~MASK;
				return b == MARKER;
			}
			static char32_t decode(byte codeUnit) {
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
			static byte extractAndEncode(char32_t& codePoint) {
				byte continuationByte = Continuation::MARKER
				        |  (codePoint & Continuation::MASK);
				codePoint >>= Continuation::CODE_UNIT_BITS;
				return continuationByte;
			}
			static void decodeAndInsert(byte codeUnit, char32_t& codePoint) {
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


CodeUnitsCount utf8::Encoder::encode(char32_t ch, CodeUnits& codeUnits) {
	using namespace utf8_impl;

	byte firstByteMarker;
	CodeUnitsCount codeUnitsCount;
	if(ch <= OneByte::MAX_CODE_POINT) {
		codeUnitsCount = 1;
		firstByteMarker = OneByte::Byte::MARKER;
	} else if(ch <= TwoBytes::MAX_CODE_POINT) {
		codeUnitsCount = 2;
		firstByteMarker = TwoBytes::Leading::MARKER;
	} else if(ch <= ThreeBytes::MAX_CODE_POINT) {
		codeUnitsCount = 3;
		firstByteMarker = ThreeBytes::Leading::MARKER;
	} else if(ch <= FourBytes::MAX_CODE_POINT) {
		codeUnitsCount = 4;
		firstByteMarker = FourBytes::Leading::MARKER;
	} else {
		throw InvalidCodePoint(ch);
	}

	for(int i = codeUnitsCount - 1; i > 0; i--) {
		codeUnits[i] = Continuation::extractAndEncode(ch);
	}
	codeUnits[0] = ch | firstByteMarker;

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

auto utf8::byteType(CodeUnit codeUnit) noexcept -> ByteType {
	using namespace utf8_impl;

	if(OneByte::Byte::matches(codeUnit)) {
		return ByteType::ASCII;
	} else if(Continuation::matches(codeUnit)) {
		return ByteType::CONTINUATION;
	} else if(TwoBytes::Leading::matches(codeUnit)) {
		return ByteType::LEADING2;
	} else if(ThreeBytes::Leading::matches(codeUnit)) {
		return ByteType::LEADING3;
	} else if(FourBytes::Leading::matches(codeUnit)  &&  codeUnit <= FourBytes::Leading::MAX_VALUE) {
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
