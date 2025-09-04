#include "math/v3.hpp"
#include "math/common.hpp"

#include <iostream>

std::ostream &operator<<(std::ostream& stream, const V3 &vector) {
	// print vector as <x,y,z>
	stream << '<' << vector[0] << ", " << vector[1] << ", " << vector[2] << '>';
	return stream;
}

// overload >> for reading V3
std::istream &operator>>(std::istream& stream, V3 &vector) {

	// try to read the opening bracket
	// skipping leading whitespace
	while (stream.peek() == ' ') stream.get();
	if (stream.get() != '<') goto error;

	// read x
	stream >> vector[0];

	// eat comma
	if (stream.get() != ',') goto error;
	
	// read y
	stream >> vector[1];

	// eat comma
	if (stream.get() != ',') goto error;

	// read z
	stream >> vector[2];

	// try to get end bracket
	if (stream.get() != '>') goto error;

error:
	// TODO: should we do anything here?
	return stream;
}

// choose auxillary axis for cross product
inline V3 ChooseAxis(const V3 &a) {
	// a dot xaxis = a.x
	// a dot yaxis = a.y
	// so we can just check a.x and a.y instead of dot product

	return std::abs(a.x()) < std::abs(a.y()) ? V3(1, 0, 0) : V3(0, 1, 0);
}

V3 V3::RotateAroundAxis(const V3 &origin, const V3 &direction, float degrees) const {

	V3 pPrime = *this - origin;
	V3 a = direction.Normalized();

	float radians = degrees * Deg2Rad;
	float sin = std::sin(radians), cos = std::cos(radians);

	// Rodrigues' formula
	V3 pPrimePrime = pPrime * cos + (a ^ pPrime) * sin + a * (a * pPrime) * (1 - cos);

	// shift back to original coordinate system
	return pPrimePrime + origin;

	/* first try, by the slides
	// step 1: construct new coordinate system
	V3 a = direction.Normalized();
	V3 b = ChooseAxis(a).Cross(a).Normalized();
	V3 c = a.Cross(b);

	// step 2: convert self to new coordinate system
	V3 Pt = *this - origin;
	V3 Pprime = M3(a, b, c) * Pt;

	// step 3: rotate about a by degrees
	

	// step 4: convert to original coordinate system
	*/

}
