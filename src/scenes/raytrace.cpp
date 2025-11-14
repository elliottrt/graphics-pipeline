#include "raytrace.hpp"
#include "color.hpp"
#include "frame_buffer.hpp"
#include "math/v3.hpp"
#include "ppcamera.hpp"
#include "scene.hpp"
#include "mesh.hpp"
#include <cfloat>
#include <iostream>
#include <utility>

RayTraceScene::RayTraceScene(WindowGroup &group):
	Scene(group), wind(group.AddWindow(640, 480, "raytrace-scene")),
	camera(wind->w, wind->h, 60.0f), line(0), order(1)
{
	meshes.push_back(std::make_pair(Mesh(), nullptr));
	meshes.back().first.Load("geometry/teapot1K.bin");
	meshes.back().first.TranslateTo(V3(0, 0, -150));

	meshes.emplace_back(Mesh(), new FrameBuffer());
	meshes.back().first.LoadPlane(V3(), V3(100, 0, 100), V3(1, 0, 0.5));
	meshes.back().first.TranslateTo(V3(0, -25, -150));
	*meshes.back().second = FrameBuffer::CreateChecker(512, 512, 16);

	// set to white initially
	wind->fb.Clear(ColorFromRGB(255, 255, 255));
}

void RayTraceScene::Update(void) {

}

void RayTraceScene::Render(void) {
	if (line >= wind->h) return;

	RayHit hit;

	int v = line;

	V3 O = camera.C, r;
	V3 color;

	//for (int v = 0; v < wind->fb.h; v++) {
	for (int u = 0; u < wind->fb.w; u++) {

		r = GetRay(u, v);
		IntersectRayWithWorld(O, r, hit);
		color = hit.hit ? hit.color : V3();

		wind->fb.SetPixel(u, v, ColorFromV3(color / order));
	}
	//}

	line++;
}

V3 RayTraceScene::GetRay(int u, int v) const {
	return (camera.c + camera.a * (u + 0.5) + camera.b * (v + 0.5)).Normalized();
}

void RayTraceScene::IntersectRayWithWorld(const V3 &O, const V3 &r, RayHit &hit) const {
	hit.hit = false;
	hit.t = FLT_MAX;

	for (const auto &mt : meshes) {
		IntersectRayWithMesh(O, r, mt.first, mt.second, hit);
	}
}

void RayTraceScene::IntersectRayWithMesh(const V3 &O, const V3 &r, const Mesh &m, const FrameBuffer *tex, RayHit &hit) const {

	M3 M;
	V3 Q0, Q1, abc;
	float t, tx, ty;

	for (size_t i = 0; i < m.triangleCount; i++) {
		const unsigned int *tri = &m.triangles[i * 3];

		M = M3::FromColumns(m.vertices[tri[0]], m.vertices[tri[1]], m.vertices[tri[2]]);
		M = M.Inverse();

		Q0 = M * O;
		Q1 = M * r;
		t = (1 - (Q0.x() + Q0.y() + Q0.z())) / (Q1.x() + Q1.y() + Q1.z());

		if (t < 0) continue;

		// compute barycentric coordinates
		abc = Q0 + Q1 * t;

		if (abc.x() < 0 || abc.y() < 0 || abc.z() < 0) continue;

		if (t >= hit.t) continue;

		hit.hit = true;
		hit.t = t;

		hit.position =
			m.vertices[tri[0]] * abc.x() +
			m.vertices[tri[1]] * abc.y() +
			m.vertices[tri[2]] * abc.z();

		hit.normal = (
			m.normals[tri[0]] * abc.x() +
			m.normals[tri[1]] * abc.y() +
			m.normals[tri[2]] * abc.z()).Normalized();

		hit.color =
			m.colors[tri[0]] * abc.x() +
			m.colors[tri[1]] * abc.y() +
			m.colors[tri[2]] * abc.z();

		if (m.tcs != nullptr && tex != nullptr) {
			tx =
				m.tcs[2*tri[0]+0] * abc.x() +
				m.tcs[2*tri[1]+0] * abc.y() +
				m.tcs[2*tri[2]+0] * abc.z();
			ty =
				m.tcs[2*tri[0]+1] * abc.x() +
				m.tcs[2*tri[1]+1] * abc.y() +
				m.tcs[2*tri[2]+1] * abc.z();
			hit.color = tex->GetColor(tx, ty);
		} else {
			hit.color =
				m.colors[tri[0]] * abc.x() +
				m.colors[tri[1]] * abc.y() +
				m.colors[tri[2]] * abc.z();
		}
	}

}
