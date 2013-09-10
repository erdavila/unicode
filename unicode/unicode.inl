namespace unicode {


template <typename FromEncoding, typename ToEncoding, typename IStream>
inline auto InputStream<FromEncoding, ToEncoding, IStream>::get() -> CodeUnitOrEof {
	if(nextCodeUnitIndex >= availableCodeUnits) {
		if(finished) {
			return Eof;
		}

		char32_t codePoint;
		do {
			typename std::char_traits<InputCodeUnit>::int_type codeUnit = is.get();
			using input_char_traits = typename std::char_traits<InputCodeUnit>;
			if(codeUnit == input_char_traits::eof()) {
				finished = true;
				if(decoder.partial()) {
					throw IncompleteInput();
				}
				return Eof;
			}
			codePoint = decoder.decode(codeUnit);
		} while(codePoint == FromEncoding::PartiallyDecoded);

		availableCodeUnits = encoder.encode(codePoint, codeUnits);
		nextCodeUnitIndex = 0;
	}

	return codeUnits[nextCodeUnitIndex++];
}


}
