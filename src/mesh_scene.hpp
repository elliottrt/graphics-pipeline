#ifndef MESH_SCENE_HPP
#define MESH_SCENE_HPP

#include "scene.hpp"
#include "mesh.hpp"
#include "window.hpp"

#include <vector>
#include <memory>

struct MeshScene : public Scene {
	Window &wind;
	std::vector<std::unique_ptr<Mesh>> meshes;

	MeshScene(Window &wind): wind(wind) {};

	virtual void Update() = 0;
	virtual void Render() = 0;

	inline void Update(Window &) { Update(); }
	inline void Render(Window &) { Render(); }
};

#endif
