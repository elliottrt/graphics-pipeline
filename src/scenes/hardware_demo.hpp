#ifndef SCENE_HARDWARE_DEMO_HPP
#define SCENE_HARDWARE_DEMO_HPP

#include "frame_buffer.hpp"
#include "mesh.hpp"
#include "ppcamera.hpp"
#include "scene.hpp"

struct HardwareDemoScene: public Scene {

	std::shared_ptr<Window> wind;
	Mesh mesh;
	PPCamera camera;
	unsigned int texId;
	FrameBuffer tex;

	HardwareDemoScene(WindowGroup &group);

	void Update(void) override;
	void Render(void) override;

};

#endif
