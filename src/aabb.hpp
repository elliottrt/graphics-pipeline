#ifndef AABB_HPP
#define AABB_HPP

#include <cfloat>

#include "math/v3.hpp"

struct AABB {

	V3 min, max;

	constexpr AABB(): min(FLT_MAX, FLT_MAX, FLT_MAX), max(-FLT_MAX, -FLT_MAX, -FLT_MAX) {}
	constexpr AABB(const V3 &v): min(v), max(v) {}
	constexpr AABB(const V3 &min, const V3 &max): min(min), max(max) {}

	constexpr V3 GetPosition() const { return min; }
	constexpr V3 GetSize() const { return max - min; }

	constexpr void AddPoint(const V3 &p) {
		for (size_t i = 0; i < 3; i++)
			if (p[i] < min[i]) min[i] = p[i];
		for (size_t i = 0; i < 3; i++)
			if (p[i] > max[i]) max[i] = p[i];
	}

};

#endif
