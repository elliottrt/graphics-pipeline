#ifndef SCENE_HARDWARE_DEMO_HPP
#define SCENE_HARDWARE_DEMO_HPP

#include "frame_buffer.hpp"
#include "gl.hpp"
#include "mesh.hpp"
#include "ppcamera.hpp"
#include "scene.hpp"

#include <sstream>

struct HardwareDemoScene: public Scene {

	std::shared_ptr<Window> wind;
	Mesh filledTexMesh, wireTexMesh, filledColorMesh, wireColorMesh, floorMesh;
	Mesh uiMesh;
	PPCamera camera;
	HWTexID texId, uiTex;
	FrameBuffer tex;
	bool fill;

	bool cameraSaveKeyDown, cameraPlayPathKeyDown;

	bool playingPath;
	float pathProgress;
	std::stringstream pathStream;
	PPCamera frameStartCamera, frameEndCamera;

	HardwareDemoScene(WindowGroup &group);

	void Update(void) override;
	void Render(void) override;

};

#endif
