#ifndef SCENE_SHADER_DEMO_HPP
#define SCENE_SHADER_DEMO_HPP

#include "frame_buffer.hpp"
#include "gl.hpp"
#include "mesh.hpp"
#include "ppcamera.hpp"
#include "scene.hpp"
#include "shader.hpp"

#include <sstream>

struct ShaderDemoScene: public Scene {

	std::shared_ptr<Window> wind;
	Mesh filledColorMesh, floorMesh;
	Mesh uiMesh;
	PPCamera camera;
	HWTexID uiTex;
	ShaderProgram shader;

	GLint cameraMatrixLocation;
	GLint cameraPositionLocation;

	ShaderDemoScene(WindowGroup &group);

	void Update(void) override;
	void Render(void) override;

};

#endif
