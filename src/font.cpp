#include "font.hpp"
#include "font8x8_basic.h"

const uint8_t *FontGetChar(char c) {
	if (c >= 0) {
		return font8x8_basic[(int) c];
	} else {
		return font8x8_basic[0];
	}
}
