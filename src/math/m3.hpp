#ifndef MATH_M3_HPP
#define MATH_M3_HPP

#include "math/v3.hpp"
#include "math/common.hpp"

#include <ostream>
#include <istream>
#include <algorithm>
#include <cmath>

struct M3 {
	V3 rows[3];

	// default constructor
	constexpr M3(): rows{} {}
	// constructor from matrix rows
	constexpr M3(const V3 &row1, const V3 &row2, const V3 &row3): rows{row1, row2, row3} {}
	// constructor from 9 different values
	constexpr M3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22)
		: rows{{m00, m01, m02}, {m10, m11, m12}, {m20, m21, m22}} {}

	// identity matrix
	constexpr static M3 Identity() {
		M3 mat;
		mat[0][0] = 1.f;
		mat[1][1] = 1.f;
		mat[2][2] = 1.f;
		return mat;
	}

	// rotation matrix around basic axes by degrees
	// https://en.wikipedia.org/wiki/Rotation_matrix#Basic_3D_rotations
	static M3 RotationX(float degrees) {
		float radians = degrees * Deg2Rad;
		float sin = std::sin(radians), cos = std::cos(radians);
		return M3(
			1, 0, 0,
			0, cos, -sin,
			0, sin, cos
		);
	}
	static M3 RotationY(float degrees) {
		float radians = degrees * Deg2Rad;
		float sin = std::sin(radians), cos = std::cos(radians);
		return M3(
			cos, 0, sin,
			0, 1, 0,
			-sin, 0, cos
		);
	}
	static M3 RotationZ(float degrees) {
		float radians = degrees * Deg2Rad;
		float sin = std::sin(radians), cos = std::cos(radians);
		return M3(
			cos, -sin, 0,
			sin, cos, 0,
			0, 0, 1
		);
	}

	// read-only access to rows
	constexpr const V3 &operator[](int index) const { return rows[index]; }
	// read-write access to rows
	constexpr V3 &operator[](int index) { return rows[index]; }

	// returns a new matrix containing the transpose of this matrix
	M3 Transpose() const {
		M3 mat = *this;
		mat.TransposeInPlace();
		return mat;
	}

	// transposes this matrix in-place
	void TransposeInPlace() {
		/*
		A B C		A D G
		D E F	->	B E H
		G H I		C F I
		*/
		std::swap(rows[0][1], rows[1][0]);
		std::swap(rows[0][2], rows[2][0]);
		std::swap(rows[1][2], rows[2][1]);
	}

	// matrix-scalar multiplication
	constexpr M3 operator*(const float &o) const {
		return M3(
			rows[0] * o,
			rows[1] * o,
			rows[2] * o
		);
	}

	// matrix-vector multiplication
	// note that we have vec * vec = vec dot vec
	constexpr V3 operator*(const V3 &o) const {
		return V3(
			rows[0] * o,
			rows[1] * o,
			rows[2] * o
		);
	}

	// matrix-matrix multiplication
	M3 operator*(const M3 &o) const {
		M3 columns = o.Transpose();
		return M3(
			rows[0] * columns[0], rows[0] * columns[1], rows[0] * columns[2],
			rows[1] * columns[0], rows[1] * columns[1], rows[1] * columns[2],
			rows[2] * columns[0], rows[2] * columns[1], rows[2] * columns[2]
		);
	}

	// returns the inverse of this matrix
	// if the matrix is not invertible, returns Identity
	// https://en.wikipedia.org/wiki/Invertible_matrix#Inversion_of_3_%C3%97_3_matrices
	constexpr M3 Inverse() const {
		// find determinant
		float determinant =
			rows[0][0] * (rows[1][1] * rows[2][2] - rows[2][1] * rows[1][2]) -
            rows[0][1] * (rows[1][0] * rows[2][2] - rows[1][2] * rows[2][0]) +
            rows[0][2] * (rows[1][0] * rows[2][1] - rows[1][1] * rows[2][0]);

		// make sure we can actually reverse the matrix
		if (determinant == 0.0f) {
			return M3::Identity();
		}

		M3 adj(
			rows[1][1] * rows[2][2] - rows[1][2] * rows[2][1],
			rows[0][2] * rows[2][1] - rows[0][1] * rows[2][2],
			rows[0][1] * rows[1][2] - rows[0][2] * rows[1][1],
			rows[1][2] * rows[2][0] - rows[1][0] * rows[2][2],
			rows[0][0] * rows[2][2] - rows[0][2] * rows[2][0],
			rows[1][0] * rows[0][2] - rows[0][0] * rows[1][2],
			rows[1][0] * rows[2][1] - rows[2][0] * rows[1][1],
			rows[2][0] * rows[0][1] - rows[0][0] * rows[2][1],
			rows[0][0] * rows[1][1] - rows[1][0] * rows[0][1]
		);

		return adj * (1 / determinant);
	}

};

// overload << for printing M3
std::ostream &operator<<(std::ostream& stream, const M3 &matrix);

// overload >> for reading M3
std::istream &operator>>(std::istream& stream, M3 &matrix);

#endif
