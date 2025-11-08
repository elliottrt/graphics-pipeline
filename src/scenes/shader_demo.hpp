#ifndef SCENE_SHADER_DEMO_HPP
#define SCENE_SHADER_DEMO_HPP

#include "frame_buffer.hpp"
#include "gl.hpp"
#include "mesh.hpp"
#include "ppcamera.hpp"
#include "scene.hpp"
#include "shader.hpp"

#include <sstream>

constexpr static const size_t IMPOSTOR_COUNT = 2;

struct ShaderDemoScene: public Scene {

	std::shared_ptr<Window> wind;
	Mesh reflectiveMesh;
	Mesh uiMesh;
	PPCamera camera;
	HWTexID uiTex, floorTex;
	ShaderProgram shader;

	ShaderUniformLocation lEye;

	Mesh impostorMeshes[IMPOSTOR_COUNT];
	V3 impostorV0[IMPOSTOR_COUNT];
	V3 impostorV1[IMPOSTOR_COUNT];
	V3 impostorV2[IMPOSTOR_COUNT];

	HWTexID impostorTex;
	ShaderUniformLocation lTex;
	ShaderUniformLocation lV0[IMPOSTOR_COUNT];
	ShaderUniformLocation lV1[IMPOSTOR_COUNT];
	ShaderUniformLocation lV2[IMPOSTOR_COUNT];

	ShaderDemoScene(WindowGroup &group);

	void Update(void) override;
	void Render(void) override;

};

#endif
