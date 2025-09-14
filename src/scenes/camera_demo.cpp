#include "camera_demo.hpp"
#include "color.hpp"

#include <iostream>

CameraDemoScene::CameraDemoScene(Window &wind): camera(wind.w, wind.h, 60.f) {
	teapot.Load(RES_DIR"/geometry/teapot1K.bin");

	std::cout << "center of mass = " << teapot.GetCenter() << std::endl;

	// go backwards to see teapot
	camera.Translate(V3(0.0f, 30.0f, 150.0f));
}

void CameraDemoScene::Update(Window &wind) {
	teapot.RotateAroundAxis(teapot.GetCenter(), V3(0.0f, 1.0f, 0.0f), 90.f * wind.deltaTime);
}

void CameraDemoScene::Render(Window &wind) {
	wind.Clear(0);
	//teapot.DrawVertices(wind, camera, 5, ColorFromRGB(255, 255, 255));
	teapot.DrawWireframe(wind, camera);
}
