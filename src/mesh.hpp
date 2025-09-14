#ifndef MESH_HPP
#define MESH_HPP

#include "math/v3.hpp"
#include "window.hpp"
#include "ppcamera.hpp"

struct Mesh {
	V3 *vertices;
	size_t vertexCount;
	V3 *colors; // vertex colors in V3 format
				// (one float in [0.0f, 1.0f] per R, G, and B channel)

	// triples of vertices
	unsigned int *triangles;
	size_t triangleCount;

	// creates an empty mesh
	Mesh();

	V3 GetCenter(void) const;

	void RotateAroundAxis(const V3 &origin, const V3 &axis, float theta);

	// TODO: get from professor
	void Load(const std::string &path);

	// draw only the vertices
	void DrawVertices(Window &wind, const PPCamera &camera, size_t pointSize, uint32_t color);

	// TODO: draw as a wireframe
	// void DrawWireframe(Window &wind);
};

#endif
