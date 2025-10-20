#include "cube_map.hpp"
#include "ppcamera.hpp"

CubeMap::CubeMap(const std::array<std::string, N> &sides):
	cameras(), buffers(), lookupFace(0)
{
	for (size_t i = 0; i < N; i++) {
		buffers[i].LoadFromTiff(sides[i].c_str());
		cameras[i] = PPCamera(buffers[i].w, buffers[i].h, 90.0f);
	}

	// camera 0 - forwards, no change
	// camera 1 - left
	cameras[1].RotateAroundDirection(V3(0, 1, 0), 90.0f);
	// camera 2 - back
	cameras[2].RotateAroundDirection(V3(0, 1, 0), 180.0f);
	// camera 3 - right
	cameras[3].RotateAroundDirection(V3(0, 1, 0), 270.0f);
	// camera 4 - up
	cameras[4].RotateAroundDirection(V3(1, 0, 0), 90.0f);
	// camera 5 - down
	cameras[5].RotateAroundDirection(V3(1, 0, 0), -90.0f);
}

CubeMap::CubeMap() {}

V3 CubeMap::Lookup(const V3 &direction) {
	V3 PP;

	for (size_t _i = 0; _i < N; _i++) {
		size_t i = (_i + lookupFace) % N;

		if (cameras[i].ProjectPoint(direction, PP)) {
			lookupFace = i;
			return buffers[i].GetColor(PP.x(), PP.y(), false, true);
		}
	}

	// should never get here... return a shocking color
	return V3(0.5, 0, 1);
}
