#include "cube_map.hpp"
#include "ppcamera.hpp"

CubeMap::CubeMap(const std::array<std::string, N> &sides):
	cameras(), buffers()
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

V3 CubeMap::Lookup(const V3 &direction) const {

}
