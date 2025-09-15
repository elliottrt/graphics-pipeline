#ifndef AABB_HPP
#define AABB_HPP

#include "math/v3.hpp"

struct AABB {

	V3 min, max;

	constexpr AABB(const V3 &min, const V3 &max): min(min), max(max) {}

	constexpr V3 GetPosition() const { return min; }
	constexpr V3 GetSize() const { return max - min; }

};

#endif
