#ifndef SCENE_RAYTRACE_HPP
#define SCENE_RAYTRACE_HPP

#include "frame_buffer.hpp"
#include "mesh.hpp"
#include "ppcamera.hpp"
#include "scene.hpp"
#include "window.hpp"
#include <cfloat>
#include <list>
#include <utility>

struct RayHit {
	// did the ray hit anything
	bool hit;

	// distance along the ray of the hit
	float t;

	// position of the hit
	V3 position;

	// normal of the hit
	V3 normal;

	// color at the hit
	V3 color;
};

struct RayTraceScene: public Scene {

	std::shared_ptr<Window> wind;
	PPCamera camera;

	std::list<std::pair<Mesh, FrameBuffer *>> meshes;

	int line;
	int order;

	RayTraceScene(WindowGroup &group);

	V3 GetRay(int u, int v) const;
	void IntersectRayWithWorld(const V3 &O, const V3 &r, RayHit &hit) const;
	void IntersectRayWithMesh(const V3 &O, const V3 &r, const Mesh &m, const FrameBuffer *tex, RayHit &hit) const;

	void Update(void) override;
	void Render(void) override;

};

#endif
