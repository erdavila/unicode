#include "unicode.hpp"


namespace unicode {


void utf8::Encoder::encode(char32_t, CodeUnits&, CodeUnitsCount&) {
	;
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
