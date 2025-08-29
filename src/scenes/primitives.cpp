#include "primitives.hpp"
#include "color.hpp"
#include "scene.hpp"

PrimitivesScene::PrimitivesScene(Window &wind) {
	(void) wind;
	// intentionally left empty
}

void PrimitivesScene::Update(Window &wind) {
	// intentionally left empty
	(void) wind;
}

void PrimitivesScene::Render(Window &wind) {
	wind.Clear(ColorFromRGB(0, 0, 0));

	// some rectangles
	wind.DrawRect(100, 100, 200, 300, ColorFromRGB(255, 0, 0));
	wind.DrawRect(200, 250, 300, 50, ColorFromRGB(0, 255, 0));

	// some circles
	wind.DrawCircle(wind.w / 2, wind.h, 100, ColorFromRGB(0, 0, 255));
	wind.DrawCircle(600, 450, 25, ColorFromRGB(127, 127, 127));

	// some lines
	wind.DrawLine(0, 0, 50, 100, ColorFromRGB(255, 0, 255));
	wind.DrawLine(50, 100, wind.w - 50, wind.h - 10, ColorFromRGB(0, 255, 127));

	// some triangles
	// wind.DrawTriangle(300, 300, 400, 100, 500, 300, ColorFromRGB(127, 138, 0));
	// wind.DrawTriangle(400, 400, 450, 350, 500, 400, ColorFromRGB(0, 138, 127));
}
