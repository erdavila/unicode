#ifndef UNICODE_HPP_
#define UNICODE_HPP_


#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <cassert>
#define NOT_TESTED { assert(!"not tested"); } // TEMPORARY!!!
#define NOT_IMPLEMENTED { assert(!"not implemented"); } // TEMPORARY!!!


namespace unicode {

using CodeUnitsCount = unsigned int;

enum : char32_t {
	MAX_CODE_POINT = U'\U0010FFFF',
};


class Exception : public std::logic_error {
public:
	Exception(const std::string& message) noexcept : std::logic_error(message) {}
};

class CodePointException : public Exception {
public:
	char32_t codePoint;
	CodePointException(const char* problem, char32_t codePoint) noexcept;
private:
	static std::string msg(const char* problem, char32_t codePoint) noexcept;
};

class InvalidCodePoint : public CodePointException {
public:
	InvalidCodePoint(char32_t codePoint) noexcept : CodePointException("Invalid code point", codePoint) {}
};

class InvalidCodeUnit : public Exception {
public:
	InvalidCodeUnit() noexcept : Exception("Invalid code unit") {}
};

class IncompleteInput : public Exception {
public:
	IncompleteInput() noexcept : Exception("Incomplete input") {}
};


template <typename CodeUnit_, unsigned int MaxCodeUnitsPerCodePoint_>
struct Encoding {
	using CodeUnit = CodeUnit_;
	enum { MaxCodeUnitsPerCodePoint = MaxCodeUnitsPerCodePoint_ };
	using CodeUnits = CodeUnit[MaxCodeUnitsPerCodePoint];
	enum : char32_t { PartiallyDecoded = 0xFFFFFFFF };
	using CodeUnitsCount = ::unicode::CodeUnitsCount;
	using InvalidCodePoint = ::unicode::InvalidCodePoint;
	using InvalidCodeUnit = ::unicode::InvalidCodeUnit;

	class Encoder {
	public:
		virtual ~Encoder() = default;
		virtual CodeUnitsCount encode(char32_t, CodeUnits&) = 0;
	};

	template <typename EncoderImpl>
	class _polymorphic_encoder_impl : public EncoderImpl, public Encoder {
		CodeUnitsCount encode(char32_t codePoint, CodeUnits& codeUnit) override { return EncoderImpl::encode(codePoint, codeUnit); }
	};

	class Decoder {
	public:
		virtual ~Decoder() = default;
		virtual char32_t decode(CodeUnit) = 0;
		virtual bool partial() const noexcept = 0;
		virtual void reset() noexcept = 0;
	};

	template <typename DecoderImpl>
	class _polymorphic_decoder_impl : public DecoderImpl, public Decoder {
		char32_t decode(CodeUnit codeUnit) override { return DecoderImpl::decode(codeUnit); }
		bool partial() const noexcept override { return DecoderImpl::partial(); }
		void reset() noexcept override { DecoderImpl::reset(); }
	};
};


struct utf8 : public Encoding<char, 4> {
	using EncodingBase = Encoding<char, 4>;

	class OverlongEncoding : public CodePointException {
	public:
		OverlongEncoding(char32_t codePoint) noexcept : CodePointException("Overlong encoding of code point", codePoint) {}
	};

	class UnexpectedContinuationByte : public Exception {
	public:
		UnexpectedContinuationByte() noexcept : Exception("Unexpected continuation byte") {}
	};

	class ExpectedContinuationByte : public Exception {
	public:
		ExpectedContinuationByte() noexcept : Exception("Expected continuation byte") {}
	};

	using InvalidByte = InvalidCodeUnit;


	class Encoder {
	public:
		CodeUnitsCount encode(char32_t, CodeUnits&);
	};
	using PolymorphicEncoder = _polymorphic_encoder_impl<Encoder>;

	class Decoder {
	public:
		char32_t decode(CodeUnit);
		bool partial() const noexcept;
		void reset() noexcept;
	private:
		int state = 0;
		int pending;
		char32_t decoding;
	};
	using PolymorphicDecoder = _polymorphic_decoder_impl<Decoder>;

	enum class ByteType {
		ASCII, CONTINUATION,
		LEADING2, LEADING3, LEADING4,
		INVALID
	};

	static ByteType byteType(char b) noexcept;
};


struct utf32 : public Encoding<char32_t, 1> {
	using EncodingBase = Encoding<char32_t, 1>;

	class Encoder {
	public:
		CodeUnitsCount encode(char32_t, CodeUnits&);
	};
	using PolymorphicEncoder = _polymorphic_encoder_impl<Encoder>;

	class Decoder {
	public:
		char32_t decode(CodeUnit);
		bool partial() const noexcept;
		void reset() noexcept;
	};
	using PolymorphicDecoder = _polymorphic_decoder_impl<Decoder>;
};


struct utf32be : public Encoding<char, 4> {
	using EncodingBase = Encoding<char, 4>;

	class Encoder {
	public:
		CodeUnitsCount encode(char32_t, CodeUnits&);
	};
	using PolymorphicEncoder = _polymorphic_encoder_impl<Encoder>;

	class Decoder {
	public:
		char32_t decode(CodeUnit);
		bool partial() const noexcept;
		void reset() noexcept;
	private:
		int count = 0;
		char32_t decoding = 0;
	};
	using PolymorphicDecoder = _polymorphic_decoder_impl<Decoder>;
};


template <typename FromEncoding, typename ToEncoding, typename IStream>
class InputStream {
public:
	using InputCodeUnit  = typename FromEncoding::CodeUnit;
	using OutputCodeUnit = typename   ToEncoding::CodeUnit;

	using char_traits = std::char_traits<OutputCodeUnit>;
	using CodeUnitOrEof = typename char_traits::int_type;

	enum : CodeUnitOrEof { Eof = char_traits::eof() };

	InputStream(IStream& is) noexcept : is(is) {}

	CodeUnitOrEof get();
	bool eof() const noexcept { return finished; }

private:
	typename FromEncoding::Decoder decoder;
	typename ToEncoding::Encoder encoder;
	IStream& is;
	bool finished = false;
	typename ToEncoding::CodeUnits codeUnits;
	typename ToEncoding::CodeUnitsCount availableCodeUnits = 0;
	unsigned int nextCodeUnitIndex = 0;
};


template <typename FromEncoding, typename ToEncoding, typename OStream>
class OutputStream {
public:
	using InputCodeUnit  = typename FromEncoding::CodeUnit;
	using OutputCodeUnit = typename   ToEncoding::CodeUnit;

	OutputStream(OStream& os) noexcept : os(os) {}

	void put(InputCodeUnit);

private:
	OStream& os;
};


template <typename FromEncoding, typename ToEncoding, typename IStream>
inline InputStream<FromEncoding, ToEncoding, IStream> makeInputStream(IStream& is) noexcept {
	return InputStream<FromEncoding, ToEncoding, IStream>(is);
}

template <typename FromEncoding, typename ToEncoding, typename OStream>
inline OutputStream<FromEncoding, ToEncoding, OStream> makeOutputStream(OStream& os) noexcept {
	return OutputStream<FromEncoding, ToEncoding, OStream>(os);
}


}


#include "unicode.inl"


#endif /* UNICODE_HPP_ */
