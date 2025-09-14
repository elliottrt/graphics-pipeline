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

	// store this so we don't waste time recomputing it,
	// we only update this when the model is modified
	V3 centerOfMass;

	// creates an empty mesh
	Mesh();

	V3 GetCenter(void) const;

	void UpdateCenterOfMass(void);

	// move the model by the given delta
	void Translate(const V3 &delta);
	// move the model's center of mass to the given world position
	void TranslateTo(const V3 &position);

	// uniform scaling around center of mass
	void Scale(const float &scale);

	void RotateAroundAxis(const V3 &origin, const V3 &axis, float theta);

	// set this mesh to empty
	void Reset(void);

	// load model from a binary file
	void Load(const std::string &path);

	// create AABB from center position and dimensions
	void LoadAABB(const V3 &center, const V3 &dimensions, const V3 &color);

	// draw only the vertices
	void DrawVertices(Window &wind, const PPCamera &camera, size_t pointSize, uint32_t color);
	// draw lines between the vertices
	void DrawWireframe(Window &wind, const PPCamera &camera);

	void SetTriangle(size_t index, unsigned int v0, unsigned int v1, unsigned int v2);
	
};

#endif
