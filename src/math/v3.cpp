#include "math/v3.hpp"
#include "math/m3.hpp"

#include <iostream>

std::ostream &operator<<(std::ostream& stream, const V3 &vector) {
	return stream << vector[0] << ' ' << vector[1] << ' ' << vector[2];
}

// overload >> for reading V3
std::istream &operator>>(std::istream& stream, V3 &vector) {
	return stream >> vector[0] >> vector[1] >> vector[2];
}

// choose auxillary axis for cross product
inline V3 ChooseAxis(const V3 &a) {
	// a dot xaxis = a.x
	// a dot yaxis = a.y
	// so we can just check a.x and a.y instead of dot product

	return fabsf(a[0]) < fabsf(a[1]) ? V3(1, 0, 0) : V3(0, 1, 0);
}

V3 V3::RotateAroundAxis(const V3 &Oa, const V3 &ad, float theta) const {

	///* in class code
	M3 lcs;
	lcs[1] = ad; // placing arbitrary axis ad as second or "y" axis 
						// of the local coordinate system lcs
	V3 aux = ChooseAxis(ad); // auxiliary axis used to build lcs
	lcs[0] = (aux ^ ad).Normalized(); // now lcs has both its x and y set
	lcs[2] = lcs[0] ^ lcs[1]; // z is always x cross y (for a right handed coordinate system)

	// 2. Transform point to lcs
	V3 P1 = lcs*(*this - Oa);

	// 3. Rotate P1 in lcs about y, that's where we put the arbitrary axis
	V3 P2 = M3::RotationY(theta) * P1;

	// 4. Go back to world CS
	V3 ret = lcs.Inverse()*P2 + Oa;

	return ret;
	//*/

	/* my work for assignment 2
	V3 pPrime = *this - Oa;
	V3 a = ad.Normalized();

	float radians = theta * Deg2Rad;
	float sin = std::sin(radians), cos = std::cos(radians);

	// Rodrigues' formula
	V3 pPrimePrime = pPrime * cos + (a ^ pPrime) * sin + a * (a * pPrime) * (1 - cos);

	// shift back to original coordinate system
	return pPrimePrime + Oa;
	*/

	/* first try, by the slides
	// step 1: construct new coordinate system
	V3 a = ad.Normalized();
	V3 b = ChooseAxis(a).Cross(a).Normalized();
	V3 c = a.Cross(b);

	// step 2: convert self to new coordinate system
	V3 Pt = *this - Oa;
	V3 Pprime = M3(a, b, c) * Pt;

	// step 3: rotate about a by degrees
	

	// step 4: convert to original coordinate system
	*/

}
