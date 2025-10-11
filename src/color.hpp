#ifndef COLOR_HPP
#define COLOR_HPP

#include <cstdint>

#include "math/v3.hpp"

// basic color utility functions

constexpr inline uint32_t ColorFromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return (a << 24) | (b << 16) | (g << 8) | r;
}

constexpr inline uint32_t ColorFromRGB(uint8_t r, uint8_t g, uint8_t b) {
	return (0xFF << 24) | (b << 16) | (g << 8) | r;
}

constexpr inline uint32_t ColorFromV3(const V3 &v) {
	return ColorFromRGB(
		(uint8_t)(v[0] * UINT8_MAX),
		(uint8_t)(v[1] * UINT8_MAX),
		(uint8_t)(v[2] * UINT8_MAX));
}

constexpr inline V3 V3FromColor(uint32_t c) {
	uint8_t r = (c >>  0) & 0xFF;
	uint8_t g = (c >>  8) & 0xFF;
	uint8_t b = (c >> 16) & 0xFF;

	return V3(r, g, b) / UINT8_MAX;
}

constexpr inline uint32_t ColorFromInverseZ(float invZ) {
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

constexpr inline uint32_t ColorAlpha(uint32_t c) {
	return (c >> 24) & 0xFF;
}

constexpr inline uint32_t ColorRed(uint32_t c) {
	return (c >> 0) & 0xFF;
}

constexpr inline uint32_t ColorGreen(uint32_t c) {
	return (c >> 8) & 0xFF;
}

constexpr inline uint32_t ColorBlue(uint32_t c) {
	return (c >> 16) & 0xFF;
}

constexpr inline uint32_t ColorBlend4(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
	auto red = (ColorRed(a) + ColorRed(b) + ColorRed(c) + ColorRed(d)) / 4;
	auto green = (ColorGreen(a) + ColorGreen(b) + ColorGreen(c) + ColorGreen(d)) / 4;
	auto blue = (ColorBlue(a) + ColorBlue(b) + ColorBlue(c) + ColorBlue(d)) / 4;
	auto alpha = (ColorAlpha(a) + ColorAlpha(b) + ColorAlpha(c) + ColorAlpha(d)) / 4;
	return ColorFromRGBA(red, green, blue, alpha);
}

#endif // COLOR_HPP
