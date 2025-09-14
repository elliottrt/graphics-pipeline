#include "math/m3.hpp"

std::ostream &operator<<(std::ostream& stream, const M3 &matrix) {
	return stream << matrix[0] << matrix[1] << matrix[2];
}

std::istream &operator>>(std::istream& stream, M3 &matrix) {
	return stream >> matrix[0] >> matrix[1] >> matrix[2];
}

M3 M3::RotationX(float degrees) {
	float radians = degrees * Deg2Rad;
	float sin = std::sin(radians), cos = std::cos(radians);
	return M3(
		1, 0, 0,
		0, cos, -sin,
		0, sin, cos
	);
}

M3 M3::RotationY(float degrees) {
	float radians = degrees * Deg2Rad;
	float sin = std::sin(radians), cos = std::cos(radians);
	return M3(
		cos, 0, sin,
		0, 1, 0,
		-sin, 0, cos
	);
}

M3 M3::RotationZ(float degrees) {
	float radians = degrees * Deg2Rad;
	float sin = std::sin(radians), cos = std::cos(radians);
	return M3(
		cos, -sin, 0,
		sin, cos, 0,
		0, 0, 1
	);
}
