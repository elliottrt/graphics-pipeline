#ifndef MATH_V3_HPP
#define MATH_V3_HPP

#include <cmath>
#include <ostream>
#include <istream>

// I'm using C++'s constexpr to speed up computation where possible
// but it requires all constexpr declarations to be in the header file
struct V3 {
	float xyz[3];

	// default, uninitialized vector
	constexpr V3(): V3(0.f, 0.f, 0.f) {}
	// three element constructor
	constexpr V3(float x, float y, float z): xyz{x, y, z} {}

	// named element access, read only
	constexpr const float &x() const { return xyz[0]; }
	constexpr const float &y() const { return xyz[1]; }
	constexpr const float &z() const { return xyz[2]; }

	// named element access, read/write
	constexpr float &x() { return xyz[0]; }
	constexpr float &y() { return xyz[1]; }
	constexpr float &z() { return xyz[2]; }

	// element read access
	constexpr const float &operator[](int index) const {
		return xyz[index];
	}

	// element write access
	constexpr float &operator[](int index) {
		return xyz[index];
	}

	// vector addition
	constexpr V3 operator+(const V3 &o) const {
		V3 ret;
		for (int i = 0; i < 3; i++) {
			ret[i] = this->xyz[i] + o[i];
		}
		return ret;
	}

	// vector assignment addition, for Window::DrawTriangle
	constexpr V3 &operator+=(const V3 &o) {
		*this = *this + o;
		return *this;
	}

	// vector subtraction
	constexpr V3 operator-(const V3 &o) const {
		V3 ret;
		for (int i = 0; i < 3; i++) {
			ret[i] = this->xyz[i] - o[i];
		}
		return ret;
	}

	constexpr V3 &operator-=(const V3 &o) {
		*this = *this - o;
		return *this;
	}

	// unary minus
	constexpr V3 operator-() const {
		return V3(-xyz[0], -xyz[1], -xyz[2]);
	}

	// vector scalar multiplication
	constexpr V3 operator*(const float &o) const {
		V3 ret;
		for (int i = 0; i < 3; i++) {
			ret[i] = this->xyz[i] * o;
		}
		return ret;
	}

	// vector scalar division
	constexpr V3 operator/(const float &o) const {
		V3 ret;
		for (int i = 0; i < 3; i++) {
			ret[i] = this->xyz[i] / o;
		}
		return ret;
	}

	constexpr V3 &operator/=(const float &o) {
		*this = *this / o;
		return *this;
	}

	// explicit dot product
	constexpr float Dot(const V3 &o) const {
		return xyz[0] * o[0] + xyz[1] * o[1] + xyz[2] * o[2];
	}

	// dot product as operator
	inline float operator*(const V3 &o) const {
		return Dot(o);
	}

	// sometimes we can optimize with square length,
	// which doesn't do the square root and is faster
	constexpr float SquareLength() const {
		return xyz[0] * xyz[0] + xyz[1] * xyz[1] + xyz[2] * xyz[2];
	}

	// length of the vector
	float Length() const {
		return std::sqrtf(SquareLength());
	}

	// similar function as SquareLength, might be useful later
	constexpr float SquareDistanceTo(const V3 &o) const {
		return (o - *this).SquareLength();
	}

	// distance between two points
	float DistanceTo(const V3 &o) {
		return (o - *this).Length();
	}

	// returns the normalized version of this vector,
	// doesn't modify self
	constexpr V3 Normalized() const {
		return *this / Length();
	}

	// returns unit direction to another vector
	constexpr V3 DirectionTo(const V3 &o) {
		return (o - *this).Normalized();
	}

	// explicit cross product
	constexpr V3 Cross(const V3 &o) const {
		return V3(
			y() * o.z() - z() * o.y(),
			z() * o.x() - x() * o.z(),
			x() * o.y() - y() * o.x()
		);
	}

	// cross product as operator
	inline V3 operator^(const V3 &o) {
		return Cross(o);
	}

	// rotate this vector around an arbitrary axis and origin
	V3 RotateAroundAxis(const V3 &Oa, const V3 &ad, float theta) const;

	// rotate this direction around an arbitrary direction
	inline V3 RotateAroundDirection(const V3 &direction, float degrees) const {
		return RotateAroundAxis(V3(), direction, degrees);
	}

	// interpolate two vectors
	inline constexpr V3 Interpolate(const V3 &o, const float &t) const {
		return *this + (o - *this) * t;
	}

};

// overload << for printing V3
std::ostream &operator<<(std::ostream& stream, const V3 &vector);

// overload >> for reading V3
std::istream &operator>>(std::istream& stream, V3 &vector);

#endif
