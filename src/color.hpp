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
	return ColorFromRGB(
		(uint8_t)(v[0] * UINT8_MAX),
		(uint8_t)(v[1] * UINT8_MAX),
		(uint8_t)(v[2] * UINT8_MAX));
}

constexpr uint32_t ColorFromInverseZ(float invZ) {
	/*
	mapping invZ
	0 -> 0
	inf -> 1
	*/

	constexpr float scale = 0.1f;
	const float z = 1.0f - (1.0f / (1.0f + invZ * scale));
	uint8_t i = (uint8_t)(z * UINT8_MAX);

	return ColorFromRGB(i, i, i);
}

#endif // COLOR_HPP
