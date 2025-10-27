#include "ppcamera.hpp"
#include "math/common.hpp"
#include "math/v3.hpp"
#include "math/m3.hpp"
#include "gl.hpp"

#include <OpenGL/gl.h>
#include <iostream>
#include <fstream>
#include <sstream>

PPCamera::PPCamera(int w, int h, float hfov): w(w), h(h), hfov(hfov) {
	C = V3(0, 0, 0);
	a = V3(1, 0, 0);
	b = V3(0, -1, 0);

	float hfovRadians = hfov * Deg2Rad;
	float focalLength = w/(2 * std::tanf(hfovRadians / 2.f));

	c = V3(-w/2.f, h/2.f, -focalLength);
	Update();
}

void PPCamera::Update() {
	MInv = M3::FromColumns(a, b, c).Inverse();
}

V3 PPCamera::GetViewDirection(void) const {
	return a.Cross(b).Normalized();
}

float PPCamera::GetFocalLength(void) const {
	return GetViewDirection().Dot(c);
}

void PPCamera::Zoom(const float &factor) {
	V3 vd = GetViewDirection();
	float PPu = -c*a.Normalized()/a.Length();
	float PPv = -c*b.Normalized()/b.Length();

	c = a * -PPu - b * PPv + vd * (vd * c) * factor;

	Update();
}

PPCamera PPCamera::Interpolate(const PPCamera &o, float t) const {
	PPCamera out(w, h, hfov);

	// simple interpolation of position
	out.C = C + (o.C - C) * t;
	out.a = (a + (o.a - a) * t).Normalized();

	V3 vdi = GetViewDirection() + (o.GetViewDirection() - GetViewDirection()) * t;

	float PPu = -c*a.Normalized()/a.Length();
	float PPv = -c*b.Normalized()/b.Length();

	out.b = vdi.Cross(out.a).Normalized();
	out.c = -out.a * PPu - out.b * PPv + vdi * GetFocalLength();

	out.Update();
	return out;
}

bool PPCamera::ProjectPoint(const V3 &P, V3 &projectedP) const {
	// q = <u, v, 1> * x
	V3 q = MInv * (P - C);

	// behind the eye, bad point
	// note: if this is instead 0.0f, there is extreme lag at certain points
	if (q.z() <= 0.001f) {
		return false;
	}

	// projectedP = <u/w, v/w, 1/w>
	// 1/w is useful for z-buffering
	projectedP.x() = q.x() / q.z();
	projectedP.y() = q.y() / q.z();
	projectedP.z() = 1.0f  / q.z();

	// success!
	return true;
}

V3 PPCamera::UnprojectPoint(int u, int v, float invZ) const {
	return C + (
		a * (0.5f + (float) u) +
		b * (0.5f + (float) v) +
		c
	) / invZ;
}

void PPCamera::TranslateGlobal(const V3 &delta) {
	C += delta;
}

void PPCamera::TranslateLocal(const V3 &delta) {
	C += a.Normalized() * delta.x();
	C -= b.Normalized() * delta.y();
	C -= a.Cross(b).Normalized() * delta.z();
}

void PPCamera::RotateAroundDirection(const V3 &direction, const float &degrees) {
	a = a.RotateAroundDirection(direction, degrees);
	b = b.RotateAroundDirection(direction, degrees);
	c = c.RotateAroundDirection(direction, degrees);
	Update();
}

void PPCamera::Pan(const float &degrees) {
	const V3 ad = -b.Normalized();
	a = a.RotateAroundDirection(ad, degrees);
	c = c.RotateAroundDirection(ad, degrees);
	Update();
}

void PPCamera::Tilt(const float &degrees) {
	RotateAroundDirection(a.Normalized(), degrees);
}

void PPCamera::Roll(const float &degrees) {
	RotateAroundDirection(GetViewDirection(), degrees);
}

void PPCamera::Pose(const V3 &newC, const V3 &lookAtPoint, const V3 &up) {
	V3 newa, newb, newc;

	const V3 newViewDirection = (lookAtPoint - newC).Normalized();
	newa = newViewDirection.Cross(up).Normalized();
	newb = newViewDirection.Cross(newa).Normalized();
	newc = newViewDirection * GetFocalLength() - newa * ((float)w/2.0f) - newb * ((float)h/2.0f);

	a = newa, b = newb, c = newc, C = newC;
	Update();
}

void PPCamera::SaveToFile(const std::string &path) {
	std::ofstream o(path);
	if (o.good()) o << *this;
	else std::cerr << "unable to save " << path << std::endl;
}

void PPCamera::LoadFromFile(const std::string &path) {
	std::ifstream i(path);
	if (i.good()) i >> *this;
	else std::cerr << "unable to load " << path << std::endl;
	Update();
}

void PPCamera::LoadFromString(const std::string &str) {
	std::stringstream(str) >> *this;
	Update();
}

void PPCamera::InitializeGL(float near, float far) const {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float scaleF = near / GetFocalLength();
	float wf = a.Length() * w;
	float hf = b.Length() * h;

	glFrustum(-wf/2.0f*scaleF, wf/2.0f*scaleF, -hf/2.0f*scaleF, hf/2.0f*scaleF, near, far);
	glMatrixMode(GL_MODELVIEW);
}

void PPCamera::SetGLView() const {
	V3 eye = C;
	V3 look = C + GetViewDirection();
	V3 down = b.Normalized();
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2], look[0], look[1], look[2], -down[0], -down[1], -down[2]);
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
	stream
		>> camera.a
		>> camera.b
		>> camera.c
		>> camera.C
		>> camera.w
		>> camera.h;
	camera.Update();
	return stream;
}
