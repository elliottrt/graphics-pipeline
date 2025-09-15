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

	V3 GetViewDirection(void) const;
	float GetFocalLength(void) const;

	// TODO: check this
	void Zoom(const float &factor);

	inline void ZoomIn() { Zoom(1.1); }
	inline void ZoomOut() { Zoom(1 / 1.1); }

	// returns true if the point is within view
	bool ProjectPoint(const V3 &P, V3 &projectedP) const;

	// translate globally, relative to world
	void TranslateGlobal(const V3 &delta);
	// translate relative to self
	// TODO: check if this works
	void TranslateLocal(const V3 &delta);

	void RotateAroundDirection(const V3 direction, const float &degrees);

	void Pan(const float &degrees);
	void Tilt(const float &degrees);
	void Roll(const float &degrees);

};

// overload << for saving PPCamera
std::ostream &operator<<(std::ostream& stream, const PPCamera &camera);

// overload >> for loading PPCamera
std::istream &operator>>(std::istream& stream, PPCamera &camera);

#endif
