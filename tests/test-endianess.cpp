#include "test.hpp"
#include <initializer_list>
#include <iterator>
#include <cstdint>
using namespace std;
using namespace unicode;


template <typename Endianness>
void testEncode(int value, std::initializer_list<char> expectedBytes) {
	assert(expectedBytes.size() == Endianness::NumBytes);

	const Endianness endianness(value);
	auto it = expectedBytes.begin();
	for(int i = 0; i < Endianness::NumBytes; i++) {
		char expectedByte = *it;
		ASSERT_EQ(expectedByte, endianness.getByte(i));
		it++;
	}
}

template <typename Endianness>
void testDecode(int expectedValue, std::initializer_list<char> bytes) {
	assert(bytes.size() == Endianness::NumBytes);

	Endianness endianness;
	auto it = bytes.begin();
	for(int i = 0; i < Endianness::NumBytes; i++) {
		char byte = *it;
		endianness.setByte(i, byte);
		it++;
	}
	ASSERT_EQ(expectedValue, endianness.value);
}


TEST(BigEndianTest, Encode) {
	testEncode<BigEndian<int_least32_t, 4>>(0xA1B2C3D4, {'\xA1', '\xB2', '\xC3', '\xD4'});
}

TEST(BigEndianTest, Decode) {
	testDecode<BigEndian<int_least32_t, 4>>(0xA1B2C3D4, {'\xA1', '\xB2', '\xC3', '\xD4'});
}

TEST(LittleEndianTest, Encode) {
	testEncode<LittleEndian<int_least32_t, 4>>(0xA1B2C3D4, {'\xD4', '\xC3', '\xB2', '\xA1'});
}

TEST(LittleEndianTest, Decode) {
	testDecode<LittleEndian<int_least32_t, 4>>(0xA1B2C3D4, {'\xD4', '\xC3', '\xB2', '\xA1'});
}
