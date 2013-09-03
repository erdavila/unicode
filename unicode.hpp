#ifndef UNICODE_HPP_
#define UNICODE_HPP_


namespace unicode {

using byte = unsigned char;
using CodeUnitsCount = unsigned int;


template <typename CodeUnit_, unsigned int MaxCodeUnitsPerCodePoint_>
struct Encoding {
	using CodeUnit = CodeUnit_;
	enum { MaxCodeUnitsPerCodePoint = MaxCodeUnitsPerCodePoint_ };
	using CodeUnits = CodeUnit[MaxCodeUnitsPerCodePoint];
	enum : char32_t { Partial = 0xFFFFFFFF };
	using CodeUnitsCount = ::unicode::CodeUnitsCount;

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
	};
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
