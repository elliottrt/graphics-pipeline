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

	// load from side paths
	CubeMap(const std::array<std::string, N> &sides);

	V3 Lookup(const V3 &direction) const;
};

#endif