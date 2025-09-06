#ifndef FONT_HPP
#define FONT_HPP

#include <cstdint>

const uint8_t *FontGetChar(char c);

constexpr inline unsigned FontSize() {
	return 8;
}

#endif
