#include "camera_demo.hpp"
#include "color.hpp"

#include <iostream>

CameraDemoScene::CameraDemoScene(Window &wind): camera(wind.w, wind.h, 60.f) {
	teapot.Load(RES_DIR"/geometry/teapot1K.bin");
	teapot.TranslateTo(V3(0, 0, -100));
}

void CameraDemoScene::Update(Window &wind) {
	teapot.RotateAroundAxis(teapot.GetCenter(), V3(0.0f, 1.0f, 0.0f), 90.f * wind.deltaTime);
}

void CameraDemoScene::Render(Window &wind) {
	wind.Clear(0);
	// teapot.DrawVertices(wind, camera, 5);
	teapot.DrawWireframe(wind, camera);
}
