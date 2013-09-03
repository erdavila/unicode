#ifndef UNICODE_HPP_
#define UNICODE_HPP_


namespace unicode {

using byte = unsigned char;
using CodeUnitsCount = unsigned int;


template <typename CodeUnit_, unsigned int MaxCodeUnitsPerCodePoint_>
struct EncodingBase {
	using CodeUnit = CodeUnit_;
	enum { MaxCodeUnitsPerCodePoint = MaxCodeUnitsPerCodePoint_ };
	using CodeUnits = CodeUnit[MaxCodeUnitsPerCodePoint];

	class EncoderBase {
	public:
		virtual ~EncoderBase() = default;
		virtual void encode(char32_t, CodeUnits&, CodeUnitsCount&) = 0;
	};

	class DecoderBase {
	public:
		enum : char32_t { Partial = 0xFFFFFFFF };
		virtual ~DecoderBase() = default;
		virtual char32_t decode(CodeUnit) = 0;
	};
};


struct utf8 : public EncodingBase<byte, 4> {
	using EncodingBaseType = EncodingBase<byte, 4>;

	class Encoder : EncodingBaseType::EncoderBase {
	public:
		void encode(char32_t, CodeUnits&, CodeUnitsCount&) override;
	};

	class Decoder : EncodingBaseType::DecoderBase {
		char32_t decode(CodeUnit) override;
	};
};


struct utf32 : public EncodingBase<char32_t, 1> {
	using EncodingBaseType = EncodingBase<char32_t, 1>;

	class Encoder : EncodingBaseType::EncoderBase {
		void encode(char32_t, CodeUnits&, CodeUnitsCount&) override;
	};

	class Decoder : EncodingBaseType::DecoderBase {
		char32_t decode(CodeUnit) override;
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
