#include "camera_demo.hpp"
#include "color.hpp"

CameraDemoScene::CameraDemoScene(Window &wind): camera(wind.w, wind.h, 90.f) {
	teapot.Load("geometry/teapot1K.bin");
}

void CameraDemoScene::Update(Window &wind) {
	(void) wind;
}

void CameraDemoScene::Render(Window &wind) {
	wind.Clear(0);
	teapot.DrawVertices(wind, camera, 5, ColorFromRGB(255, 255, 255));
}
