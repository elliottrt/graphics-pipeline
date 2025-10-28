#ifndef SCENE_HARDWARE_DEMO_HPP
#define SCENE_HARDWARE_DEMO_HPP

#include "frame_buffer.hpp"
#include "mesh.hpp"
#include "ppcamera.hpp"
#include "scene.hpp"

struct HardwareDemoScene: public Scene {

	std::shared_ptr<Window> wind;
	Mesh filledTexMesh, wireTexMesh, filledColorMesh, wireColorMesh, floorMesh;
	Mesh uiMesh;
	PPCamera camera;
	unsigned int texId, uiTex;
	FrameBuffer tex;
	bool fill;

	HardwareDemoScene(WindowGroup &group);

	void Update(void) override;
	void Render(void) override;

};

#endif
