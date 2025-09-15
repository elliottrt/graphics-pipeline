#include "ppcamera.hpp"
#include "math/common.hpp"
#include "math/v3.hpp"
#include "math/m3.hpp"

PPCamera::PPCamera(int w, int h, float hfov): w(w), h(h) {
	C = V3(0, 0, 0);
	a = V3(1, 0, 0);
	b = V3(0, -1, 0);

	float hfovRadians = hfov * Deg2Rad;
	float focalLength = w/(2 * std::tanf(hfovRadians / 2.f));

	c = V3(-w/2.f, h/2.f, -focalLength);

	// we need a, b, c to be columns of M, but we only have a row constructor
	// so we make them the rows then transpose them
	MInv = M3(a, b, c).Transpose().Inverse();
}

V3 PPCamera::GetViewDirection(void) const {
	return a.Cross(b).Normalized();
}

float PPCamera::GetFocalLength(void) const {
	return GetViewDirection().Dot(c);
}

void PPCamera::Zoom(const float &factor) {
	const V3 vd = GetViewDirection();
	c += vd * vd.Dot(c) * factor;
}

bool PPCamera::ProjectPoint(const V3 &P, V3 &projectedP) const {
	// q = <u, v, 1> * x
	V3 q = MInv * (P - C);

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

void PPCamera::TranslateGlobal(const V3 &delta) {
	C += delta;
}

void PPCamera::TranslateLocal(const V3 &delta) {
	C += a.Normalized() * delta.x();
	C += -b.Normalized() * delta.y();
	C += a.Cross(b).Normalized() * delta.z();
}

void PPCamera::RotateAroundDirection(const V3 direction, const float &degrees) {
	a = a.RotateAroundDirection(direction, degrees);
	b = b.RotateAroundDirection(direction, degrees);
	c = c.RotateAroundDirection(direction, degrees);
}

void PPCamera::Pan(const float &degrees) {
	RotateAroundDirection(-b, degrees);
}

void PPCamera::Tilt(const float &degrees) {
	RotateAroundDirection(a, degrees);
}

void PPCamera::Roll(const float &degrees) {
	RotateAroundDirection(c, degrees);
}

std::ostream &operator<<(std::ostream& stream, const PPCamera &camera) {
	return stream
		<< camera.a << ' '
		<< camera.b << ' '
		<< camera.c << ' '
		<< camera.C << ' '
		<< camera.w << ' '
		<< camera.h;
}

std::istream &operator>>(std::istream& stream, PPCamera &camera) {
	return stream
		>> camera.a
		>> camera.b
		>> camera.c
		>> camera.C
		>> camera.w
		>> camera.h;
}
