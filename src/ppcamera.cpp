#include "ppcamera.hpp"
#include "math/common.hpp"
#include "math/v3.hpp"
#include "math/m3.hpp"

PPCamera::PPCamera(int w, int h, float hfov): w(w), h(h) {
	position = V3(0, 0, 0);
	a = V3(1, 0, 0);
	b = V3(0, -1, 0);

	float hfovRadians = hfov * Deg2Rad;
	float focalLength = w/(2 * std::tanf(hfovRadians / 2.f));

	c = V3(-w/2.f, h/2.f, -focalLength);

	// we need a, b, c to be columns of M, but we only have a row constructor
	// so we make them the rows then transpose them
	MInv = M3(a, b, c).Transpose().Inverse();
}

bool PPCamera::ProjectPoint(const V3 &P, V3 &projectedP) {
	// q = <u, v, 1> * x
	V3 q = MInv * (P - position);

	// behind the eye, bad point
	// TODO: near clipping plane
	if (q.z() <= 0.0f) {
		return false;
	}

	// projectedP = <u/w, v/w, 1/w>
	// 1/w is useful for z-buffering
	projectedP = q / q.z();

	// success!
	return true;
}
