namespace unicode {


template <typename T, size_t NumBytes_, size_t ShiftedBytes(size_t)>
inline char _Endianness<T, NumBytes_, ShiftedBytes>::getByte(size_t index) const {
	size_t shiftedBits = 8 * ShiftedBytes(index);
	return (value >> shiftedBits) & 0xFF;
}

template <typename T, size_t NumBytes_, size_t ShiftedBytes(size_t)>
inline void _Endianness<T, NumBytes_, ShiftedBytes>::setByte(size_t index, char byte) {
	size_t shiftedBits = 8 * ShiftedBytes(index);
	value_type mask = 0xFF << shiftedBits;
	value = (value & ~mask) | ((0xFF & byte) << shiftedBits);
}

template <size_t NumBytes>
inline constexpr size_t _bigEndianShiftedBytes(size_t index) noexcept {
	return NumBytes - index - 1;
}

template <size_t NumBytes>
inline constexpr size_t _littleEndianShiftedBytes(size_t index) noexcept {
	return index;
}


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

		typename ToEncoding::Encoder encoder;
		availableCodeUnits = encoder.encode(codePoint, codeUnits);
		nextCodeUnitIndex = 0;
	}

	return codeUnits[nextCodeUnitIndex++];
}


template <typename FromEncoding, typename ToEncoding, typename OStream>
void OutputStream<FromEncoding, ToEncoding, OStream>::put(InputCodeUnit inputCodeUnit) {
	char32_t codePoint = decoder.decode(inputCodeUnit);
	if(codePoint != FromEncoding::PartiallyDecoded) {
		typename ToEncoding::Encoder encoder;
		typename ToEncoding::CodeUnits outputCodeUnits;
		typename ToEncoding::CodeUnitsCount outputCodeUnitsCount = encoder.encode(codePoint, outputCodeUnits);
		for(auto i = 0u; i < outputCodeUnitsCount; i++) {
			typename ToEncoding::CodeUnit outputCodeUnit = outputCodeUnits[i];
			os.put(outputCodeUnit);
		}
	}
}

template <typename FromEncoding, typename ToEncoding, typename OStream>
constexpr bool OutputStream<FromEncoding, ToEncoding, OStream>::incompleteInput() const noexcept {
	return decoder.partial();
}


}
