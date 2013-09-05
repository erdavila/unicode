#ifndef UNICODE_HPP_
#define UNICODE_HPP_


#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <cassert> // TEMPORARY!!!
#define NOT_TESTED { assert(!"not tested"); } // TEMPORARY!!!
#define NOT_IMPLEMENTED { assert(!"not implemented"); } // TEMPORARY!!!


namespace unicode {

using byte = unsigned char;
using CodeUnitsCount = unsigned int;


class Exception : public std::logic_error {
public:
	Exception(const std::string& message) : std::logic_error(message) {}
};

class CodePointException : public Exception {
public:
	char32_t codePoint;
	CodePointException(const char* problem, char32_t codePoint) : Exception(msg(problem, codePoint)), codePoint(codePoint) {}
private:
	static std::string msg(const char* problem, char32_t codePoint) {
		std::ostringstream ostrs;
		ostrs << problem << " U+" << std::hex << codePoint;
		return ostrs.str();
	}
};

class InvalidCodePoint : public CodePointException {
public:
	InvalidCodePoint(char32_t codePoint) : CodePointException("Invalid code point", codePoint) {}
};

class OverlongEncoding : public CodePointException {
public:
	OverlongEncoding(char32_t codePoint) : CodePointException("Overlong encoding of code point", codePoint) {}
};


template <typename CodeUnit_, unsigned int MaxCodeUnitsPerCodePoint_>
struct Encoding {
	using CodeUnit = CodeUnit_;
	enum { MaxCodeUnitsPerCodePoint = MaxCodeUnitsPerCodePoint_ };
	using CodeUnits = CodeUnit[MaxCodeUnitsPerCodePoint];
	enum : char32_t { PartiallyDecoded = 0xFFFFFFFF };
	using CodeUnitsCount = ::unicode::CodeUnitsCount;
	using InvalidCodePoint = ::unicode::InvalidCodePoint;
	using OverlongEncoding = ::unicode::OverlongEncoding;

	class Encoder {
	public:
		virtual ~Encoder() = default;
		virtual void dispatchEncode(char32_t, CodeUnits&, CodeUnitsCount&) = 0;
	};

	class Decoder {
	public:
		virtual ~Decoder() = default;
		virtual char32_t dispatchDecode(CodeUnit) = 0;
	};

	template <typename Derived>
	class EncoderBase : public Encoder {
	public:
		void dispatchEncode(char32_t ch, CodeUnits& codeUnits, CodeUnitsCount& codeUnitsCount) override {
			dynamic_cast<Derived*>(this)->encode(ch, codeUnits, codeUnitsCount);
		}
	};

	template <typename Derived>
	class DecoderBase : public Decoder {
	public:
		char32_t dispatchDecode(CodeUnit codeUnit) override {
			return dynamic_cast<Derived*>(this)->decode(codeUnit);
		}
	};
};


struct utf8 : public Encoding<byte, 4> {
	using EncodingBase = Encoding<byte, 4>;

	class Encoder : public EncodingBase::EncoderBase<Encoder> {
	public:
		void encode(char32_t, CodeUnits&, CodeUnitsCount&);
	};

	class Decoder : public EncodingBase::DecoderBase<Decoder> {
	public:
		char32_t decode(CodeUnit);
	private:
		int state = 0;
		int pending;
		char32_t decoding;
	};

	enum class ByteType {
		ASCII, CONTINUATION,
		LEADING2, LEADING3, LEADING4
	};

	static ByteType byteType(CodeUnit codeUnit);
};


struct utf32 : public Encoding<char32_t, 1> {
	using EncodingBase = Encoding<char32_t, 1>;

	class Encoder : public EncodingBase::EncoderBase<Encoder> {
	public:
		void encode(char32_t, CodeUnits&, CodeUnitsCount&);
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

	InputStream(IStream& is) : is(is) {}

	OutputCodeUnit get();

private:
	IStream& is;
};


template <typename From, typename To, typename OStream>
class OutputStream {
public:
	using InputCodeUnit  = typename From::CodeUnit;
	using OutputCodeUnit = typename   To::CodeUnit;

	OutputStream(OStream& os) : os(os) {}

	void put(InputCodeUnit);

private:
	OStream& os;
};


template <typename From, typename To, typename IStream>
inline InputStream<From, To, IStream> createInputStream(IStream& is) {
	return InputStream<From, To, IStream>(is);
}

template <typename From, typename To, typename OStream>
inline OutputStream<From, To, OStream> createOutputStream(OStream& os) {
	return OutputStream<From, To, OStream>(os);
}


}


#endif /* UNICODE_HPP_ */
