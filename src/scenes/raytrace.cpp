#include "raytrace.hpp"
#include "aabb.hpp"
#include "color.hpp"
#include "frame_buffer.hpp"
#include "math/v3.hpp"
#include "ppcamera.hpp"
#include "scene.hpp"
#include "mesh.hpp"

#include <cfloat>
#include <iostream>
#include <numeric>

RayBVHNode::RayBVHNode(uint32_t index, uint32_t count):
	index(index), count(count)
{

}

void RayBVH::Construct(const std::array<RayObject, OBJECT_COUNT> &objects) {
	size_t triCount = 0;
	for (const auto &o : objects) triCount += o.mesh.triangleCount;

	nodes.clear();
	nodes.reserve(triCount);

	objIndices.clear();
	objIndices.resize(triCount, 0);
	std::iota(objIndices.begin(), objIndices.end(), 0);

	nodes.emplace_back(0, triCount);
	UpdateBounds(0, objects);
	Subdivide(0, objects);

	std::cout << "bvh: " << nodes.size() << " nodes created from " << triCount << " triangles" << std::endl;
	PrintTree();
	// exit(0);
}

void RayBVH::PrintTree(size_t index, size_t depth) const {
	if (index < nodes.size()) {
		const RayBVHNode &node = nodes.at(index);
		std::cout << std::string(depth, ' ') << "node: min=<" << node.aabb.min << ">, max=<" << node.aabb.max << ">";

		if (node.count > 0) {
			std::cout << ", " << node.count << " objects" << std::endl;
		} else {
			std::cout << std::endl;
			PrintTree(node.index + 0, depth + 1);
			PrintTree(node.index + 1, depth + 1);
		}
	}
}

void RayBVH::DrawTree(FrameBuffer &fb, const PPCamera &camera, size_t index) const {
	const RayBVHNode &node = nodes[index];

	// TODO: better color?

	const float cdiv = std::min(0.25f + 1.0f / (floorf(log2f(node.index + 1)) + 1.0f), 1.0f);

	fb.DrawAABB(camera, node.aabb, V3(1, 1, 1) * cdiv);

	if (node.count == 0) {
		DrawTree(fb, camera, node.index + 0);
		DrawTree(fb, camera, node.index + 1);
	}
}

std::pair<const RayObject &, size_t> RayBVH::GetTri(const std::array<RayObject, OBJECT_COUNT> &objects, size_t index_) const {
	auto index = objIndices[index_];

	for (const RayObject &o : objects) {
		if (index < o.mesh.triangleCount) {
			return {o, index};
		} else {
			index -= o.mesh.triangleCount;
		}
	}
	assert(false && "invalid tri index");
}

V3 RayBVH::GetTriCenter(const std::array<RayObject, OBJECT_COUNT> &objects, size_t index) const {
	const auto [obj, triIndex] = GetTri(objects, index);
	const auto tri = &obj.mesh.triangles[triIndex*3];
	const auto verts = obj.mesh.vertices;

	return (verts[tri[0]] + verts[tri[1]] + verts[tri[2]]) / 3.0f;
}

void RayBVH::UpdateBounds(size_t nodeIndex, const std::array<RayObject, OBJECT_COUNT> &objects) {
	RayBVHNode &node = nodes.at(nodeIndex);

	node.aabb.min = V3(FLT_MAX, FLT_MAX, FLT_MAX);
	node.aabb.max = -node.aabb.min;

	for (size_t i = 0; i < node.count; i++) {
		const auto [obj, triIndex] = GetTri(objects, node.index + i);
		const auto tri = &obj.mesh.triangles[triIndex * 3];

		node.aabb.AddPoint(obj.mesh.vertices[tri[0]]);
		node.aabb.AddPoint(obj.mesh.vertices[tri[1]]);
		node.aabb.AddPoint(obj.mesh.vertices[tri[2]]);
	}

	std::cout<<"after "<<node.count<<" tris, we have min="<<node.aabb.min<<",max="<<node.aabb.max<<std::endl;
}

void RayBVH::Subdivide(size_t nodeIndex, const std::array<RayObject, OBJECT_COUNT> &objects, bool diffAxis) {
	RayBVHNode &node = nodes.at(nodeIndex);

	std::cerr << "maybe splitting node with " << node.count << " children" << std::endl;

	if (node.count <= 2) return; // don't create unreasonable children

	const V3 aabbSize = node.aabb.max - node.aabb.min;
	int axis = 0;
	if (aabbSize.y() > aabbSize.x()) axis = 1;
    if (aabbSize.z() > aabbSize[axis]) axis = 2;

	if (diffAxis) {
		axis = (axis + 1) % 3;
	}

    float splitPos = node.aabb.min[axis] + aabbSize[axis] / 2;

	// partition objects

	int i = node.index;
    int j = i + node.count - 1;

	std::cout << "start: axis="<<axis<<",splitPos="<<splitPos<<",i="<<i<<",j="<<j<<std::endl;

    while (i <= j)
    {
        if (GetTriCenter(objects, i)[axis] < splitPos) {
			i++;
		} else {
            std::swap(objIndices[i], objIndices[j]);
			j -= 1;
		}
    }

	std::cout << "end  : i="<<i<<",j="<<j<<std::endl;

	// don't split if one of the resulting nodes would be empty

	std::cout << "left = " << i - node.index << ", right = " << node.count - (i - node.index) << std::endl;

	size_t leftCount = i - node.index;
    if (leftCount == 0 || leftCount == node.count) return;

	// create child nodes

	auto leftIndex = nodes.size();
	nodes.emplace_back(node.index, leftCount);

	auto rightIndex = nodes.size();
	nodes.emplace_back(i, node.count - leftCount);

    node.index = leftIndex;
    node.count = 0;

    UpdateBounds(leftIndex, objects);
    UpdateBounds(rightIndex, objects);

	// recurse, fixing case where parent is same size as child

	Subdivide(leftIndex, objects, nodes[leftIndex].aabb == node.aabb);
	Subdivide(rightIndex, objects, nodes[rightIndex].aabb == node.aabb);

}

void RayHit::Reset() {
	hit = false;
	obj = nullptr;
	t = FLT_MAX;
#ifdef RAY_STATS
	trisChecked = 0;
	bvhsChecked = 0;
#endif // RAY_STATS
}

RayTraceScene::RayTraceScene(WindowGroup &group):
	Scene(group), wind(group.AddWindow(640, 480, "raytrace-scene")),
	camera(wind->w, wind->h, 60.0f), threads(8), order(0)
{
	objects[0].mesh.Load("geometry/teapot1K.bin");
	objects[0].mesh.TranslateTo(V3(0, 10, -100));
	objects[0].mesh.Scale(0.25f);
	objects[0].reflective = true;

	objects[1].mesh.LoadPlane(V3(0, -25, -100), V3(1, 0, 1)*50, V3(1, 0, 0.5));
	objects[1].fb = new FrameBuffer;
	*objects[1].fb = FrameBuffer::CreateChecker(512, 512, 16);

	// set to white initially
	wind->fb.Clear(ColorFromRGB(255, 255, 255));

	bvh.Construct(objects);
}

void RayTraceScene::Update(void) {

	bool useGlobal = wind->KeyPressed(SDL_SCANCODE_G);

	V3 movement;
	movement.x() = (float)wind->KeyPressed(SDL_SCANCODE_D) - (float)wind->KeyPressed(SDL_SCANCODE_A);
	movement.y() = (float)wind->KeyPressed(SDL_SCANCODE_SPACE) - (float)(wind->KeyPressed(SDL_SCANCODE_LSHIFT) || wind->KeyPressed(SDL_SCANCODE_RSHIFT));
	movement.z() = (float)wind->KeyPressed(SDL_SCANCODE_S) - (float)wind->KeyPressed(SDL_SCANCODE_W);

	if (useGlobal)
		camera.TranslateGlobal(movement * wind->deltaTime * 10);
	else
		camera.TranslateLocal(movement * wind->deltaTime * 10);

	// rotation

	V3 rotation;
	rotation.x() = (float)wind->KeyPressed(SDL_SCANCODE_UP) - (float)wind->KeyPressed(SDL_SCANCODE_DOWN);
	rotation.y() = (float)wind->KeyPressed(SDL_SCANCODE_LEFT) - (float)wind->KeyPressed(SDL_SCANCODE_RIGHT);

	if (rotation.x() != 0.0f) camera.Tilt(rotation.x() * wind->deltaTime * 45);
	if (rotation.y() != 0.0f) camera.Pan(rotation.y() * wind->deltaTime * 45);

	if (wind->KeyPressed(SDL_SCANCODE_R)) {
		if (!renderJustChanged) {
			renderBVH = !renderBVH;
			renderJustChanged = true;
		}
	} else {
		renderJustChanged = false;
	}

}

static void RenderLines(RayTraceScene *s, int start, int end, int id) {
	RayHit hit;
	V3 O, r;
	int o;
	V3 color;

#ifdef RAY_STATS
	size_t trisChecked = 0;
	size_t bvhsChecked = 0;
#endif // RAY_STATS

	(void) id;
	// std::cout << "starting " << id << std::endl;

	for (int v = start; v < end; v++) {
	for (int u = 0; u < s->wind->fb.w; u++) {

		O = s->camera.C;
		r = s->GetRay(u, v);
		s->IntersectRayWithWorld(O, r, hit);

#ifdef RAY_STATS
		trisChecked += hit.trisChecked;
		bvhsChecked += hit.bvhsChecked;
#endif // RAY_STATS

		o = 1;
		color = hit.hit ? hit.color : V3();

		for (; o < s->order && hit.hit && hit.obj->reflective; o++) {
			r = hit.normal.Reflect(-r);
			O = hit.position + r * 1e-3;

			s->IntersectRayWithWorld(O, r, hit);
			if (hit.hit) color += hit.color;

#ifdef RAY_STATS
			trisChecked += hit.trisChecked;
			bvhsChecked += hit.bvhsChecked;
#endif // RAY_STATS
		}

		s->wind->fb.SetPixel(u, v, ColorFromV3(color / o));
	}
	}

#ifdef RAY_STATS
	size_t raysCast = size_t(s->wind->fb.w) * size_t(end - start);
	std::cout << "raystats: " << raysCast << " rays, " << bvhsChecked << " aabbs checked, " << trisChecked << " tris checked" << std::endl;
	std::cout << "          " << bvhsChecked / (double) raysCast << " avg aabbs checked, " << trisChecked / (double) raysCast << " avg tris checked" << std::endl;
#endif // RAY_STATS

}

void RayTraceScene::Render(void) {
	std::cout << "dt=" << wind->deltaTime << ", ft=" << wind->frameTime << std::endl;

	int start = 0;
	int delta = wind->fb.h / std::max(threads.size(), 1ul);
	int index = 0;

	if (renderBVH) {
		wind->fb.Clear(0);
		bvh.DrawTree(wind->fb, camera);
		return;
	}

	if (threads.size() > 0) {
		for (auto &t : threads) {
			t = std::thread(RenderLines, this, start, start + delta, index++);
			start += delta;
		}

		for (auto &t : threads) if (t.joinable()) t.join();
	} else {
		RenderLines(this, 0, wind->fb.h, 0);
	}
}

V3 RayTraceScene::GetRay(int u, int v) const {
	return (camera.c + camera.a * (u + 0.5) + camera.b * (v + 0.5)).Normalized();
}

void RayTraceScene::IntersectRayWithWorld(const V3 &O, const V3 &r, RayHit &hit) const {
	hit.Reset();

#if 0
	for (const auto &obj : objects) {
		IntersectRayWithObject(O, r, obj, hit);
	}
#else
	IntersectRayWithBVHNode(O, r, 0, hit);
#endif
}

void RayTraceScene::IntersectRayWithObject(const V3 &O, const V3 &r, const RayObject &obj, RayHit &hit) const {

	M3 M;
	V3 Q0, Q1, abc;
	float t, tx, ty;

	const Mesh &m = obj.mesh;
	const FrameBuffer *tex = obj.fb;

#ifdef RAY_STATS
	hit.trisChecked += m.triangleCount;
#endif // RAY_STATS

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
		hit.obj = &obj;

		hit.position =
			m.vertices[tri[0]] * abc.x() +
			m.vertices[tri[1]] * abc.y() +
			m.vertices[tri[2]] * abc.z();

		hit.normal = (
			m.normals[tri[0]] * abc.x() +
			m.normals[tri[1]] * abc.y() +
			m.normals[tri[2]] * abc.z()).Normalized();

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

// see https://jacco.ompf2.com/2022/04/13/how-to-build-a-bvh-part-1-basics/
bool RayTraceScene::IntersectRayWithAABB(const V3 &O, const V3 &r, const AABB &aabb, RayHit &hit) const {
	float
		tx1 = (aabb.min.x() - O.x()) / r.x(),
		tx2 = (aabb.max.x() - O.x()) / r.x(),
		ty1 = (aabb.min.y() - O.y()) / r.y(),
		ty2 = (aabb.max.y() - O.y()) / r.y(),
		tz1 = (aabb.min.z() - O.z()) / r.z(),
		tz2 = (aabb.max.z() - O.z()) / r.z();

	// TODO: could use std::minmax here
	float
		tmin = std::max({ std::min(tx1, tx2), std::min(ty1, ty2), std::min(tz1, tz2) }),
		tmax = std::min({ std::max(tx1, tx2), std::max(ty1, ty2), std::max(tz1, tz2) });

#ifdef RAY_STATS
	hit.bvhsChecked++;
#endif // RAY_STATS

	return tmax >= tmin && tmin < hit.t && tmax > 0;
}

void RayTraceScene::IntersectRayWithBVHNode(const V3 &O, const V3 &r, size_t nodeIndex, RayHit &hit) const {
	const RayBVHNode &node = bvh.nodes[nodeIndex];

	if (IntersectRayWithAABB(O, r, node.aabb, hit)) {
		if (node.count > 0) {
			for (size_t i = 0; i < node.count; i++) {
				const auto tri = bvh.GetTri(objects, node.index + i);
				IntersectRayWithTri(O, r, tri, hit);
			}
		} else {
			IntersectRayWithBVHNode(O, r, node.index + 0, hit);
			IntersectRayWithBVHNode(O, r, node.index + 1, hit);
		}
	}
}

void RayTraceScene::IntersectRayWithTri(const V3 &O, const V3 &r, const std::pair<const RayObject &, size_t> &p, RayHit &hit) const {

	M3 M;
	V3 Q0, Q1, abc;
	float t, tx, ty;

	const auto [obj, triIndex] = p;

	const auto &m = obj.mesh;
	const auto *tex = obj.fb;

#ifdef RAY_STATS
	hit.trisChecked += 1;
#endif // RAY_STATS

	const unsigned int *tri = &m.triangles[triIndex * 3];

	M = M3::FromColumns(m.vertices[tri[0]], m.vertices[tri[1]], m.vertices[tri[2]]);
	M = M.Inverse();

	Q0 = M * O;
	Q1 = M * r;
	t = (1 - (Q0.x() + Q0.y() + Q0.z())) / (Q1.x() + Q1.y() + Q1.z());

	if (t < 0) return;

	// compute barycentric coordinates
	abc = Q0 + Q1 * t;

	if (abc.x() < 0 || abc.y() < 0 || abc.z() < 0) return;

	if (t >= hit.t) return;

	hit.hit = true;
	hit.t = t;
	hit.obj = &obj;

	hit.position =
		m.vertices[tri[0]] * abc.x() +
		m.vertices[tri[1]] * abc.y() +
		m.vertices[tri[2]] * abc.z();

	hit.normal = (
		m.normals[tri[0]] * abc.x() +
		m.normals[tri[1]] * abc.y() +
		m.normals[tri[2]] * abc.z()).Normalized();

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
