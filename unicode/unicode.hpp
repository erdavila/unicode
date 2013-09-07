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

using byte = unsigned char;
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
		virtual CodeUnitsCount virtualEncode(char32_t, CodeUnits&) = 0;
	};

	class Decoder {
	public:
		virtual ~Decoder() = default;
		virtual char32_t virtualDecode(CodeUnit) = 0;
		virtual bool virtualPartial() const noexcept = 0;
	};

	template <typename Base, typename Derived>
	struct _Caster : Base {
		Derived* derivedThis() noexcept { return dynamic_cast<Derived*>(this); }
		const Derived* derivedThis() const noexcept { return dynamic_cast<const Derived*>(this); }
	};

	template <typename Derived>
	class EncoderBase : _Caster<Encoder, Derived> {
		using _Caster<Encoder, Derived>::derivedThis;
	public:
		CodeUnitsCount virtualEncode(char32_t ch, CodeUnits& codeUnits) override {
			return derivedThis()->encode(ch, codeUnits);
		}
	};

	template <typename Derived>
	class DecoderBase : _Caster<Decoder, Derived> {
		using _Caster<Decoder, Derived>::derivedThis;
	public:
		char32_t virtualDecode(CodeUnit codeUnit) override { return dynamic_cast<Derived*>(this)->decode(codeUnit); }
		virtual bool virtualPartial() const noexcept override { return derivedThis()->partial(); }
	};
};


struct utf8 : public Encoding<byte, 4> {
	using EncodingBase = Encoding<byte, 4>;

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


	class Encoder : public EncodingBase::EncoderBase<Encoder> {
	public:
		CodeUnitsCount encode(char32_t, CodeUnits&);
	};

	class Decoder : public EncodingBase::DecoderBase<Decoder> {
	public:
		char32_t decode(CodeUnit);
		bool partial() const noexcept;
	private:
		int state = 0;
		int pending;
		char32_t decoding;
	};

	enum class ByteType {
		ASCII, CONTINUATION,
		LEADING2, LEADING3, LEADING4,
		INVALID
	};

	static ByteType byteType(CodeUnit codeUnit) noexcept;
};


struct utf32 : public Encoding<char32_t, 1> {
	using EncodingBase = Encoding<char32_t, 1>;

	class Encoder : public EncodingBase::EncoderBase<Encoder> {
	public:
		CodeUnitsCount encode(char32_t, CodeUnits&);
	};

	class Decoder : public EncodingBase::DecoderBase<Decoder> {
	public:
		char32_t decode(CodeUnit);
	};
};


template <typename From, typename To, typename IStream>
class InputStream {
public:
	using InputCodeUnit  = typename From::CodeUnit;
	using OutputCodeUnit = typename   To::CodeUnit;

	InputStream(IStream& is) noexcept : is(is) {}

	OutputCodeUnit get();

private:
	IStream& is;
};


template <typename From, typename To, typename OStream>
class OutputStream {
public:
	using InputCodeUnit  = typename From::CodeUnit;
	using OutputCodeUnit = typename   To::CodeUnit;

	OutputStream(OStream& os) noexcept : os(os) {}

	void put(InputCodeUnit);

private:
	OStream& os;
};


template <typename From, typename To, typename IStream>
inline InputStream<From, To, IStream> createInputStream(IStream& is) noexcept {
	return InputStream<From, To, IStream>(is);
}

template <typename From, typename To, typename OStream>
inline OutputStream<From, To, OStream> createOutputStream(OStream& os) noexcept {
	return OutputStream<From, To, OStream>(os);
}


}


#endif /* UNICODE_HPP_ */
