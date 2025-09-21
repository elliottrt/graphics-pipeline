#ifndef MESH_LIGHTING_SCENE_HPP
#define MESH_LIGHTING_SCENE_HPP

#include "mesh_scene.hpp"
#include "ppcamera.hpp"

struct MeshLightingScene: public MeshScene {

	PPCamera camera;

	MeshLightingScene(Window &wind);

	void Update() override;
	void Render() override;

};

#endif
