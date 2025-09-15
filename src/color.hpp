#ifndef COLOR_HPP
#define COLOR_HPP

#include <cstdint>

#include "math/v3.hpp"

// basic color utility functions

constexpr uint32_t ColorFromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return (a << 24) | (b << 16) | (g << 8) | r;
}

constexpr uint32_t ColorFromRGB(uint8_t r, uint8_t g, uint8_t b) {
	return (0xFF << 24) | (b << 16) | (g << 8) | r;
}

constexpr uint32_t ColorFromV3(const V3 &v) {
	return ColorFromRGB(v[0] * 255, v[1] * 255, v[2] * 255);
}

#endif // COLOR_HPP
