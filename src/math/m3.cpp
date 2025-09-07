#include "math/m3.hpp"

std::ostream &operator<<(std::ostream& stream, const M3 &matrix) {
	return stream << matrix[0] << matrix[1] << matrix[2];
}

std::istream &operator>>(std::istream& stream, M3 &matrix) {
	return stream >> matrix[0] >> matrix[1] >> matrix[2];
}
