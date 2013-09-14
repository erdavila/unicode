#include <array>
#include <iostream>
#include "test.hpp"
using namespace unicode;
using namespace std;

ostream& operator<<(ostream& os, DetectedEncoding de) {
#define CASE(DE) if(de == DE) { return os << #DE; }
	CASE(DetectedEncoding::UTF8);
	CASE(DetectedEncoding::UTF16BE);
	CASE(DetectedEncoding::UTF16LE);
	CASE(DetectedEncoding::UTF32BE);
	CASE(DetectedEncoding::UTF32LE_OR_UTF16LE);
	CASE(DetectedEncoding::NOT_IDENTIFIED);
#undef CASE
	return os << "????";
}

TEST(BOMDecodeTest, UTF8) {
	// U+FEFF = 1111|1110 11|111111 <-> 1110|1111, 10|111011, 10|111111
	EXPECT_EQ(DetectedEncoding::UTF8, detectBOMEncoding('\xEF', '\xBB', '\xBF', '\x00'));
	EXPECT_EQ(DetectedEncoding::UTF8, detectBOMEncoding('\xEF', '\xBB', '\xBF', '\xFF'));
}

TEST(BOMDecodeTest, UTF16BE) {
	// U+FEFF = 11111110 11111111 <-> 11111110, 11111111
	EXPECT_EQ(DetectedEncoding::UTF16BE, detectBOMEncoding('\xFE', '\xFF', '\x00', '\x00'));
	EXPECT_EQ(DetectedEncoding::UTF16BE, detectBOMEncoding('\xFE', '\xFF', '\xFF', '\xFF'));
}

TEST(BOMDecodeTest, UTF16LE) {
	// U+FEFF = 11111110 11111111 <-> 11111111, 11111110
	EXPECT_EQ(DetectedEncoding::UTF16LE, detectBOMEncoding('\xFF', '\xFE', '\x00', '\x01'));
	EXPECT_EQ(DetectedEncoding::UTF16LE, detectBOMEncoding('\xFF', '\xFE', '\x01', '\x01'));
	EXPECT_EQ(DetectedEncoding::UTF16LE, detectBOMEncoding('\xFF', '\xFE', '\xFF', '\xFF'));
}

TEST(BOMDecodeTest, UTF32BE) {
	// U+FEFF = 11111110 11111111 <-> 00000000, 00000000, 11111110, 11111111
	EXPECT_EQ(DetectedEncoding::UTF32BE, detectBOMEncoding('\x00', '\x00', '\xFE', '\xFF'));
}

TEST(BOMDecodeTest, UTF32LEorUTF16LE) {
	// U+FEFF = 11111110 11111111 <-> 11111111, 11111110, 00000000, 00000000
	EXPECT_EQ(DetectedEncoding::UTF32LE_OR_UTF16LE, detectBOMEncoding('\xFF', '\xFE', '\x00', '\x00'));
}

TEST(BOMDecodeTest, NotIdentified) {
	EXPECT_EQ(DetectedEncoding::NOT_IDENTIFIED, detectBOMEncoding('\xA1', '\xB2', '\xC3', '\xD4'));
}
