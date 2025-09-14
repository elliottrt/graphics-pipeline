#include "mesh.hpp"

#include <iostream>
#include <fstream>

Mesh::Mesh() {
	vertices = nullptr;
	vertexCount = 0;
	colors = nullptr;
	triangles = nullptr;
	triangleCount = 0;
}

V3 Mesh::GetCenter(void) const {
	V3 sum;
	for (size_t i = 0; i < vertexCount; i++) {
		sum += vertices[i];
	}
	return sum / vertexCount;
}

void Mesh::RotateAroundAxis(const V3 &origin, const V3 &axis, float theta) {

	for (size_t vi = 0; vi < vertexCount; vi++) {
		vertices[vi] = vertices[vi].RotateAroundAxis(origin, axis, theta);
	}

}

void Mesh::Load(const std::string &path) {
	V3 *normals = 0;

	std::ifstream ifs(path, std::ios::binary);
	if (ifs.fail()) {
		std::cerr << "INFO: cannot open file: " << path << std::endl;
		return;
	}

	ifs.read((char*)&vertexCount, sizeof(int));
	char yn;
	ifs.read(&yn, 1); // always xyz
	if (yn != 'y') {
		std::cerr << "INTERNAL ERROR: there should always be vertex xyz data" << std::endl;
		return;
	}
	if (vertices)
		delete vertices;
	vertices = new V3[vertexCount];

	ifs.read(&yn, 1); // cols 3 floats
	if (colors)
		delete colors;
	colors = 0;
	if (yn == 'y') {
		colors = new V3[vertexCount];
	}

	ifs.read(&yn, 1); // normals 3 floats
	if (normals)
		delete []normals;
	normals = 0;
	if (yn == 'y') {
		normals = new V3[vertexCount];
	}

	ifs.read(&yn, 1); // texture coordinates 2 floats
	float *tcs = 0; // don't have texture coordinates for now
	if (tcs)
		delete []tcs;
	tcs = 0;
	if (yn == 'y') {
		tcs = new float[vertexCount * 2];
	}


	ifs.read((char*)vertices, vertexCount * 3 * sizeof(float)); // load verts

	if (colors) {
		ifs.read((char*)colors, vertexCount * 3 * sizeof(float)); // load colors
	}

	if (normals)
		ifs.read((char*)normals, vertexCount * 3 * sizeof(float)); // load normals

	if (tcs)
		ifs.read((char*)tcs, vertexCount * 2 * sizeof(float)); // load texture coordinates

	ifs.read((char*)&triangleCount, sizeof(int));
	if (triangles)
		delete triangles;
	triangles = new unsigned int[triangleCount * 3];
	ifs.read((char*)triangles, triangleCount * 3 * sizeof(unsigned int)); // read tiangles

	ifs.close();

	std::cerr << "INFO: loaded " << vertexCount << " verts, " << triangleCount << " tris from " << std::endl << "      " << path << std::endl;
	std::cerr << "      xyz " << ((colors) ? "rgb " : "") << ((normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") << std::endl;

}

void Mesh::DrawVertices(Window &wind, const PPCamera &camera, size_t pointSize, uint32_t color) {

	for (size_t i = 0; i < vertexCount; i++) {
		wind.DrawPoint(camera, vertices[i], pointSize, color);
	}

}
