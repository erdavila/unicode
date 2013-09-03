#include "unicode.hpp"
#include <sstream>
using namespace std;
using namespace unicode;


int main() {
	utf8::Encoder encoder;
	utf8::Decoder decoder;

	istringstream istrs;
	auto is = createInputStream<utf8, utf32>(istrs);

	ostringstream ostrs;
	auto os = createOutputStream<utf32, utf8>(ostrs);
}
