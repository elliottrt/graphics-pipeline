#ifndef SCENE_ENVMAPPING_HPP
#define SCENE_ENVMAPPING_HPP

#include "cube_map.hpp"
#include "mesh.hpp"
#include "ppcamera.hpp"
#include "window.hpp"
#include "scene.hpp"
#include <memory>

struct EnvironmentMappingScene: public Scene {

	std::shared_ptr<Window> wind;
	CubeMap map;
	Mesh obj;
	PPCamera camera;

	EnvironmentMappingScene(WindowGroup &group);

	void Update(void) override;
	void Render(void) override;

};

#endif 