#ifndef PPCAMERA_HPP
#define PPCAMERA_HPP

#include "math/v3.hpp"
#include "math/m3.hpp"

struct PPCamera {
	int w, h;
	V3 C;
	V3 a, b, c;
	float hfov;

	// cached M^-1 so we don't recompute for every point
	M3 MInv;

	PPCamera(int w, int h, float hfov);

	void Update();

	V3 GetViewDirection(void) const;
	float GetFocalLength(void) const;

	void Zoom(const float &factor);

	inline void ZoomIn() { Zoom(1.1); }
	inline void ZoomOut() { Zoom(1 / 1.1); }

	PPCamera Interpolate(const PPCamera &o, float t) const;

	inline PPCamera InterpolateSmooth(const PPCamera &o, float t) const {
		// seems to be the smoothing function that Unity's Mathf.SmoothStep uses
		return Interpolate(o, t * t * (3 - 2 * t));
	}

	// returns true if the point is within view
	bool ProjectPoint(const V3 &P, V3 &projectedP) const;

	// translate globally, relative to world
	void TranslateGlobal(const V3 &delta);
	// translate relative to self
	void TranslateLocal(const V3 &delta);

	void RotateAroundDirection(const V3 direction, const float &degrees);

	void Pan(const float &degrees);
	void Tilt(const float &degrees);
	void Roll(const float &degrees);

	void SaveToFile(const std::string &path);
	void LoadFromFile(const std::string &path);

};

// overload << for saving PPCamera
std::ostream &operator<<(std::ostream& stream, const PPCamera &camera);

// overload >> for loading PPCamera
std::istream &operator>>(std::istream& stream, PPCamera &camera);

#endif
