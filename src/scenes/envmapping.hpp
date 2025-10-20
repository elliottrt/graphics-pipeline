#ifndef SCENE_ENVMAPPING_HPP
#define SCENE_ENVMAPPING_HPP

#include "cube_map.hpp"
#include "mesh.hpp"
#include "ppcamera.hpp"
#include "window.hpp"
#include "scene.hpp"

struct EnvironmentMappingScene: public Scene {

	CubeMap map;
	Mesh obj;
	PPCamera camera;

	EnvironmentMappingScene(WindowGroup &group, Window &wind);

	void Update(Window &wind) override;
	void Render(Window &wind) override;

};

#endif 