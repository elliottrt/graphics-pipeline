#ifndef MATH_V3_HPP
#define MATH_V3_HPP

// I'm using C++'s constexpr to speed up computation where possible
// but it requires all constexpr declarations to be in the header file
struct V3 {
	float xyz[3];

	constexpr V3(): V3(0.f, 0.f, 0.f) {}
	constexpr V3(float x, float y, float z): xyz{x, y, z} {}

	constexpr float x() const { return xyz[0]; }
	constexpr float y() const { return xyz[1]; }
	constexpr float z() const { return xyz[2]; }

	constexpr const float &operator[](int index) const {
		return xyz[index];
	}

	constexpr float &operator[](int index) {
		return xyz[index];
	}

	constexpr V3 operator+(const V3 &o) const {
		V3 ret;
		for (int i = 0; i < 3; i++) {
			ret[i] = this->xyz[i] + o[i];
		}
		return ret;
	}

	constexpr V3 &operator+=(const V3 &o) {
		*this = *this + o;
		return *this;
	}

	constexpr V3 operator-(const V3 &o) const {
		V3 ret;
		for (int i = 0; i < 3; i++) {
			ret[i] = this->xyz[i] - o[i];
		}
		return ret;
	}

	constexpr V3 operator*(const float &o) const {
		V3 ret;
		for (int i = 0; i < 3; i++) {
			ret[i] = this->xyz[i] * o;
		}
		return ret;
	}

	constexpr V3 operator/(const float &o) const {
		V3 ret;
		for (int i = 0; i < 3; i++) {
			ret[i] = this->xyz[i] / o;
		}
		return ret;
	}
};

#endif
