#ifndef SCENE_RAYTRACE_HPP
#define SCENE_RAYTRACE_HPP

#include "frame_buffer.hpp"
#include "mesh.hpp"
#include "ppcamera.hpp"
#include "scene.hpp"
#include "window.hpp"

#include <array>
#include <list>
#include <thread>

constexpr static const int OBJECT_COUNT = 2;

#define RAY_STATS

struct RayObject {
	Mesh mesh;
	FrameBuffer *fb = nullptr;
	bool reflective = false;

	RayObject() = default;
};

// see https://jacco.ompf2.com/2022/04/13/how-to-build-a-bvh-part-1-basics/
struct RayBVHNode {
	AABB aabb;
	uint32_t index, objCount;

	RayBVHNode() = default;
	RayBVHNode(uint32_t index, uint32_t objCount);
};

struct RayBVH {
	std::vector<RayBVHNode> nodes;
	std::array<size_t, OBJECT_COUNT> objIndices;

	void Construct(const std::array<RayObject, OBJECT_COUNT> &objects);
	void PrintTree(size_t index = 0, size_t depth = 0) const;
	void DrawTree(FrameBuffer &fb, const PPCamera &camera, size_t index = 0) const;

private:
	void UpdateBounds(size_t nodeIndex, const std::array<RayObject, OBJECT_COUNT> &objects);
	void Subdivide(size_t nodeIndex, const std::array<RayObject, OBJECT_COUNT> &objects);
};

struct RayHit {
	// did the ray hit anything
	bool hit;
	// pointer to the object the ray hit
	// nullptr if the ray hit nothing
	const RayObject *obj;

	// distance along the ray of the hit
	float t;

	// position of the hit
	V3 position;

	// normal of the hit
	V3 normal;

	// color at the hit
	V3 color;

#ifdef RAY_STATS
	// how many triangles did this ray check?
	size_t trisChecked;

	// how many bvh's did this ray check?
	size_t bvhsChecked;
#endif // RAY_STATS

	void Reset();
};

struct RayTraceScene: public Scene {

	std::shared_ptr<Window> wind;
	PPCamera camera;

	// note: this CANNOT be a vector. it causes memory issues
	std::array<RayObject, OBJECT_COUNT> objects;
	std::list<std::thread> threads;

	RayBVH bvh;

	int order;

	RayTraceScene(WindowGroup &group);

	V3 GetRay(int u, int v) const;
	void IntersectRayWithWorld(const V3 &O, const V3 &r, RayHit &hit) const;
	void IntersectRayWithObject(const V3 &O, const V3 &r, const RayObject &obj, RayHit &hit) const;

	bool IntersectRayWithAABB(const V3 &O, const V3 &r, const AABB &aabb, RayHit &hit) const;
	void IntersectRayWithBVHNode(const V3 &O, const V3 &r, size_t nodeIndex, RayHit &hit) const;

	void Update(void) override;
	void Render(void) override;

};

#endif
