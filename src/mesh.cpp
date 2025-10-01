#include "mesh.hpp"
#include "aabb.hpp"
#include "frame_buffer.hpp"
#include "math/v3.hpp"
#include "ppcamera.hpp"

#include <cmath>
#include <iostream>
#include <fstream>
#include <cfloat>

Mesh::Mesh() {
	vertices = nullptr;
	projectedVertices = nullptr;
	vertexCount = 0;
	colors = nullptr;
	normals = nullptr;
	triangles = nullptr;
	triangleCount = 0;
	tcs = nullptr;
}

Mesh::~Mesh() {
	Reset();
}

V3 Mesh::GetCenter(void) const {
	return centerOfMass;
}

void Mesh::UpdateCenterOfMass(void) {
	centerOfMass = V3();
	for (size_t i = 0; i < vertexCount; i++)
		centerOfMass += vertices[i];
	centerOfMass /= (float) vertexCount;

	// also renormalize the vertices
	if (normals) for (size_t i = 0; i < vertexCount; i++) {
		normals[i] = normals[i].Normalized();
	}
}

void Mesh::Translate(const V3 &delta) {
	for (size_t vi = 0; vi < vertexCount; vi++) {
		vertices[vi] += delta;
	}
	centerOfMass += delta;
}

void Mesh::TranslateTo(const V3 &position) {
	Translate(position - centerOfMass);
}

void Mesh::Scale(const float &scale) {
	for (size_t i = 0; i < vertexCount; i++)
		vertices[i] = (vertices[i] - centerOfMass) * scale + centerOfMass;
	// no need to update center of mass
}

void Mesh::Reset(void) {
	delete []vertices;
	vertices = nullptr;
	delete []projectedVertices;
	projectedVertices = nullptr;
	vertexCount = 0;
	delete []colors;
	colors = nullptr;
	delete []normals;
	normals = nullptr;
	delete []triangles;
	triangles = nullptr;
	triangleCount = 0;
	delete []tcs;
	tcs = nullptr;
}

void Mesh::RotateAroundAxis(const V3 &origin, const V3 &axis, float theta) {

	for (size_t vi = 0; vi < vertexCount; vi++) {
		vertices[vi] = vertices[vi].RotateAroundAxis(origin, axis, theta);
	}

	if (normals) {
		for (size_t vi = 0; vi < vertexCount; vi++) {
			normals[vi] = normals[vi].RotateAroundDirection(axis, theta);
		}
	}

	centerOfMass = centerOfMass.RotateAroundAxis(origin, axis, theta);

}

void Mesh::Load(const std::string &path) {
	std::ifstream ifs(path, std::ios::binary);
	if (ifs.fail()) {
		std::cerr << "INFO: cannot open file: " << path << std::endl;
		return;
	}

	// clear the model and deallocate its data
	Reset();

	ifs.read((char*)&vertexCount, sizeof(int));
	char yn;
	ifs.read(&yn, 1); // always xyz
	if (yn != 'y') {
		std::cerr << "INTERNAL ERROR: there should always be vertex xyz data" << std::endl;
		return;
	}

	vertices = new V3[vertexCount];

	ifs.read(&yn, 1); // cols 3 floats
	if (yn == 'y') {
		colors = new V3[vertexCount];
	}

	ifs.read(&yn, 1); // normals 3 floats
	if (yn == 'y') {
		normals = new V3[vertexCount];
	}

	ifs.read(&yn, 1); // texture coordinates 2 floats
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
	triangles = new unsigned int[triangleCount * 3];
	ifs.read((char*)triangles, triangleCount * 3 * sizeof(unsigned int)); // read tiangles

	ifs.close();

	std::cerr << "INFO: loaded " << vertexCount << " verts, " << triangleCount << " tris from " << std::endl << "      " << path << std::endl;
	std::cerr << "      xyz " << ((colors) ? "rgb " : "") << ((normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") << std::endl;

	UpdateCenterOfMass();
}

void Mesh::LoadRectangle(const V3 &center, const V3 &dimensions, const V3 &color) {

	V3 halfs = dimensions / 2;

	Reset();

	vertexCount = 8;
	vertices = new V3[vertexCount];
	colors = new V3[vertexCount];
	triangleCount = 12;
	triangles = new unsigned int[triangleCount * 3];

	vertices[0] = V3(center.x() - halfs.x(), center.y() + halfs.y(), center.z() + halfs.z());
	vertices[1] = V3(center.x() - halfs.x(), center.y() - halfs.y(), center.z() + halfs.z());
	vertices[2] = V3(center.x() + halfs.x(), center.y() - halfs.y(), center.z() + halfs.z());
	vertices[3] = V3(center.x() + halfs.x(), center.y() + halfs.y(), center.z() + halfs.z());
	vertices[4] = V3(center.x() - halfs.x(), center.y() + halfs.y(), center.z() - halfs.z());
	vertices[5] = V3(center.x() - halfs.x(), center.y() - halfs.y(), center.z() - halfs.z());
	vertices[6] = V3(center.x() + halfs.x(), center.y() - halfs.y(), center.z() - halfs.z());
	vertices[7] = V3(center.x() + halfs.x(), center.y() + halfs.y(), center.z() - halfs.z());

	for (size_t i = 0; i < vertexCount; i++) colors[i] = color;

	// front
	SetTriangle(0, 0, 1, 2);
	SetTriangle(1, 0, 2, 3);

	// top
	SetTriangle(2, 0, 3, 7);
	SetTriangle(3, 0, 7, 4);

	// bottom
	SetTriangle(4, 1, 2, 6);
	SetTriangle(5, 1, 6, 5);

	// back
	SetTriangle(6, 4, 5, 6);
	SetTriangle(7, 4, 6, 7);

	// left
	SetTriangle(8, 0, 4, 5);
	SetTriangle(9, 0, 5, 1);

	// right
	SetTriangle(10, 3, 6, 7);
	SetTriangle(11, 3, 2, 6);

	UpdateCenterOfMass();
}

void Mesh::LoadAABB(const AABB &aabb, const V3 &color) {
	V3 size = aabb.GetSize();
	LoadRectangle(aabb.GetPosition() + size / 2, size, color);
}

AABB Mesh::GetAABB(void) const {
	if (vertexCount == 0) return AABB(V3(), V3());

	V3 min = vertices[0], max = vertices[0];
	for (size_t i = 1; i < vertexCount; i++) {
		const V3 &vertex = vertices[i];

		if (vertex.x() < min.x()) min.x() = vertex.x();
		if (vertex.x() > max.x()) max.x() = vertex.x();

		if (vertex.y() < min.y()) min.y() = vertex.y();
		if (vertex.y() > max.y()) max.y() = vertex.y();

		if (vertex.z() < min.z()) min.z() = vertex.z();
		if (vertex.z() > max.z()) max.z() = vertex.z();
	}

	return AABB(min, max);
}

void Mesh::ProjectVertices(const PPCamera &camera) {
	if (vertices != nullptr && vertexCount > 0 && projectedVertices == nullptr) {
		projectedVertices = new V3[vertexCount];
	}
	if (projectedVertices && vertices && vertexCount > 0) {
		for (size_t i = 0; i < vertexCount; i++) {
			if (!camera.ProjectPoint(vertices[i], projectedVertices[i])) {
				projectedVertices[i].z() = -1.0f;
			}
		}
	}
}

constexpr static V3 DEFAULT_COLOR = V3(1.f, 1.f, 1.f);

void Mesh::DrawVertices(FrameBuffer &fb, const PPCamera &camera, size_t pointSize) const {
	for (size_t i = 0; i < vertexCount; i++) {
		fb.DrawPoint(camera, vertices[i], pointSize, colors ? colors[i] : DEFAULT_COLOR);
	}
}

void Mesh::DrawWireframe(FrameBuffer &fb, const PPCamera &camera) const {
	for (size_t i = 0; i < triangleCount; i++) {
		unsigned int *tri = &triangles[i * 3];

		V3 p[3];
		bool in0 = camera.ProjectPoint(vertices[tri[0]], p[0]);
		bool in1 = camera.ProjectPoint(vertices[tri[1]], p[1]);
		bool in2 = camera.ProjectPoint(vertices[tri[2]], p[2]);
			
		if (in0 && in1)
			fb.DrawLine(p[0], p[1], colors ? colors[tri[0]] : DEFAULT_COLOR, colors ? colors[tri[1]] : DEFAULT_COLOR);
		if (in1 && in2)
			fb.DrawLine(p[1], p[2], colors ? colors[tri[1]] : DEFAULT_COLOR, colors ? colors[tri[2]] : DEFAULT_COLOR);
		if (in0 && in1)
			fb.DrawLine(p[0], p[2], colors ? colors[tri[0]] : DEFAULT_COLOR, colors ? colors[tri[2]] : DEFAULT_COLOR);
	}
}

// fragment shader variables
static V3 Frag_p0, Frag_p1, Frag_p2;
static V3 Frag_c0, Frag_c1, Frag_c2;
static V3 Frag_n0, Frag_n1, Frag_n2;
static PPCamera Frag_camera{1, 1, 1};
static PPCamera Frag_lightCamera{1, 1, 1};
static FrameBuffer Frag_lightBuffer{1, 1};
static float Frag_ka, Frag_specularIntensity;

static V3 FragNoLight(const V3 &B) {
	return Frag_c0 * B.x() + Frag_c1 * B.y() + Frag_c2 * B.z();
}

static V3 FragPointLight(const V3 &B) {
	V3 C = Frag_c0 * B.x() + Frag_c1 * B.y() + Frag_c2 * B.z();
	const V3 N = (Frag_n0 * B.x() + Frag_n1 * B.y() + Frag_n2 * B.z()).Normalized();
	const V3 P = Frag_p0 * B.x() + Frag_p1 * B.y() + Frag_p2 * B.z();
	const V3 L = (Frag_lightCamera.C - P).Normalized();
	C = C.Light(N, L, Frag_ka);

	// specular highlight stuff
	const float k = std::max(N.Reflect(L) * (Frag_camera.C - P).Normalized(), 0.0f);
	constexpr static float CUTOFF = 0.50f;
	float specularValue = std::powf(k, Frag_specularIntensity);
	if (specularValue >= CUTOFF) C = V3(1, 1, 1) * specularValue + C * (1 - specularValue);

	return C;
}

static V3 FragPointLightShadowMap(const V3 &B) {
	V3 C = Frag_c0 * B.x() + Frag_c1 * B.y() + Frag_c2 * B.z();
	const V3 N = (Frag_n0 * B.x() + Frag_n1 * B.y() + Frag_n2 * B.z()).Normalized();
	const V3 P = Frag_p0 * B.x() + Frag_p1 * B.y() + Frag_p2 * B.z();
	const V3 L = (Frag_lightCamera.C - P).Normalized();
	C = C.Light(N, L, Frag_ka);

	// specular highlight stuff
	const float k = std::max(N.Reflect(L) * (Frag_camera.C - P).Normalized(), 0.0f);
	constexpr static float CUTOFF = 0.50f;
	float specularValue = std::powf(k, Frag_specularIntensity);
	if (specularValue >= CUTOFF) C = V3(1, 1, 1) * specularValue + C * (1 - specularValue);

	return C;
}

void Mesh::DrawFilledNoLighting(FrameBuffer &fb, const PPCamera &camera) {
	ProjectVertices(camera);

	for (size_t i = 0; i < triangleCount; i++) {
		const unsigned int *tri = &triangles[i * 3];

		const V3 &p0 = projectedVertices[tri[0]];
		const V3 &p1 = projectedVertices[tri[1]];
		const V3 &p2 = projectedVertices[tri[2]];

		if (p0.z() < 0.0f || p1.z() < 0.0f || p2.z() < 0.0f) continue;

		if (colors) {
			Frag_c0 = colors[tri[0]];
			Frag_c1 = colors[tri[1]];
			Frag_c2 = colors[tri[2]];
		}

		fb.DrawTriangle(p0, p1, p2, FragNoLight);
	}
}

// simple version
void Mesh::DrawFilledPointLight(FrameBuffer &fb, const PPCamera &camera, const V3 &lightPos, float ka, float specularIntensity) {
	ProjectVertices(camera);

	Frag_camera = camera;
	Frag_lightCamera.C = lightPos;
	Frag_ka = ka;
	Frag_specularIntensity = specularIntensity;

	for (size_t i = 0; i < triangleCount; i++) {
		const unsigned int *tri = &triangles[i * 3];

		const V3 &p0 = projectedVertices[tri[0]];
		const V3 &p1 = projectedVertices[tri[1]];
		const V3 &p2 = projectedVertices[tri[2]];

		if (p0.z() < 0.0f || p1.z() < 0.0f || p2.z() < 0.0f) continue;

		Frag_p0 = vertices[tri[0]];
		Frag_p1 = vertices[tri[1]];
		Frag_p2 = vertices[tri[2]];

		if (colors) {
			Frag_c0 = colors[tri[0]];
			Frag_c1 = colors[tri[1]];
			Frag_c2 = colors[tri[2]];
		}
		if (normals) {
			Frag_n0 = normals[tri[0]];
			Frag_n1 = normals[tri[1]];
			Frag_n2 = normals[tri[2]];
		}

		fb.DrawTriangle(p0, p1, p2, FragPointLight);
	}
}

// shadow map version
void Mesh::DrawFilledPointLight(FrameBuffer &fb, const PPCamera &camera,
	const PPCamera &lightCamera, const FrameBuffer &lightBuffer,
	float ka, float specularIntensity)
{
	ProjectVertices(camera);

	Frag_camera = camera;
	Frag_lightCamera = lightCamera;
	Frag_lightBuffer = lightBuffer;
	Frag_ka = ka;
	Frag_specularIntensity = specularIntensity;

	for (size_t i = 0; i < triangleCount; i++) {
		const unsigned int *tri = &triangles[i * 3];

		const V3 &p0 = projectedVertices[tri[0]];
		const V3 &p1 = projectedVertices[tri[1]];
		const V3 &p2 = projectedVertices[tri[2]];

		if (p0.z() < 0.0f || p1.z() < 0.0f || p2.z() < 0.0f) continue;

		Frag_p0 = vertices[tri[0]];
		Frag_p1 = vertices[tri[1]];
		Frag_p2 = vertices[tri[2]];

		if (colors) {
			Frag_c0 = colors[tri[0]];
			Frag_c1 = colors[tri[1]];
			Frag_c2 = colors[tri[2]];
		}
		if (normals) {
			Frag_n0 = normals[tri[0]];
			Frag_n1 = normals[tri[1]];
			Frag_n2 = normals[tri[2]];
		}

		fb.DrawTriangle(p0, p1, p2, FragPointLightShadowMap);
	}
}

void Mesh::DrawNormals(FrameBuffer &fb, const PPCamera &camera) const {
	if (!normals || !colors) return;

	V3 p0, p1;

	for (size_t i = 0; i < vertexCount; i++) {

		if (!camera.ProjectPoint(vertices[i], p0)) continue;
		if (!camera.ProjectPoint(vertices[i] + normals[i].Normalized() * 5, p1)) continue;

		fb.DrawLine(p0, p1, colors[i], V3(1, 1, 1));
	}

}

void Mesh::SetTriangle(size_t index, unsigned int v0, unsigned int v1, unsigned int v2) {
	if (triangles && index < triangleCount) {
		triangles[index * 3 + 0] = v0;
		triangles[index * 3 + 1] = v1;
		triangles[index * 3 + 2] = v2;
	}
}
