#ifndef MESH_LIGHTING_SCENE_HPP
#define MESH_LIGHTING_SCENE_HPP

#include "mesh.hpp"
#include "ppcamera.hpp"
#include "window.hpp"
#include "scene.hpp"

#include <memory>
#include <vector>

struct MeshLightingScene: public Scene {

	std::shared_ptr<Window> wind;
	std::vector<std::unique_ptr<Mesh>> meshes;
	PPCamera camera;
	V3 lightPosition;
	float ka;
	float specularIntensity;
	int renderMode;
	float teapotAngle;
	float lastAngle;

	MeshLightingScene(WindowGroup &group);

	void Update() override;
	void Render() override;

};

#endif
