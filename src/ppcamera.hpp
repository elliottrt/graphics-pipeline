#ifndef PPCAMERA_HPP
#define PPCAMERA_HPP

#include "math/v3.hpp"
#include "math/m3.hpp"

struct PPCamera {
	int w, h;
	V3 C;
	V3 a, b, c;

	// cached M^-1 so we don't recompute for every point
	M3 MInv;

	// TODO: movement: forwards is position + c, right is position + a, down is position + b

	PPCamera(int w, int h, float hfov);

	// returns true if the point is within view
	bool ProjectPoint(const V3 &P, V3 &projectedP) const;

	void Translate(const V3 &delta);
};

#endif
