#ifndef CUBE_MAP_HPP
#define CUBE_MAP_HPP

#include "frame_buffer.hpp"
#include "math/v3.hpp"
#include "ppcamera.hpp"
#include <array>
#include <string>

struct CubeMap {
	static const constexpr size_t N = 6;

	PPCamera cameras[N];
	FrameBuffer buffers[N];

	// index to check first
	size_t lookupFace;

	// load from side paths
	CubeMap(const std::array<std::string, N> &sides);

	CubeMap();

	V3 Lookup(const V3 &direction);
	V3 LookupPosition(const V3 &position);
};

#endif