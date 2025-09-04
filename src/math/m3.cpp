#include "math/m3.hpp"

std::ostream &operator<<(std::ostream& stream, const M3 &matrix) {
	stream << '<' << matrix[0] << ", " << matrix[1] << ", " << matrix[2] << '>';
	return stream;
}

std::istream &operator>>(std::istream& stream, M3 &matrix) {

	// try to read the opening bracket
	// skipping leading whitespace
	while (stream.peek() == ' ') stream.get();
	if (stream.get() != '<') goto error;

	// read row 1
	stream >> matrix[0];

	// eat comma
	if (stream.get() != ',') goto error;
	
	// read row 2
	stream >> matrix[1];

	// eat comma
	if (stream.get() != ',') goto error;

	// read row 3
	stream >> matrix[2];

	// try to get end bracket
	if (stream.get() != '>') goto error;

error:
	// TODO: should we do anything here?
	return stream;
}
