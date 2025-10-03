#ifndef MESH_LIGHTING_SCENE_HPP
#define MESH_LIGHTING_SCENE_HPP

#include "mesh.hpp"
#include "ppcamera.hpp"
#include "window.hpp"
#include "scene.hpp"

#include <memory>
#include <vector>

struct MeshLightingScene: public Scene {

	std::vector<std::unique_ptr<Mesh>> meshes;
	PPCamera camera;
	V3 lightPosition;
	float ka;
	float specularIntensity;
	int renderMode;
	float teapotAngle;
	float lastAngle;

	MeshLightingScene(WindowGroup &group, Window &wind);

	void Update(Window &wind) override;
	void Render(Window &wind) override;

};

#endif
