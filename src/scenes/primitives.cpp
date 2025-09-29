#include "primitives.hpp"
#include "color.hpp"

PrimitivesScene::PrimitivesScene(Window &wind) {
	(void) wind;
	// intentionally left empty
}

void PrimitivesScene::Update(Window &wind) {
	// intentionally left empty
	(void) wind;
}

void PrimitivesScene::Render(Window &wind) {
	wind.fb.Clear(ColorFromRGB(0, 0, 0));

	// some rectangles
	wind.fb.DrawRect(100, 100, 200, 300, ColorFromRGB(255, 0, 0));
	wind.fb.DrawRect(200, 250, 300, 50, ColorFromRGB(0, 255, 0));

	// some circles
	wind.fb.DrawCircle(wind.w / 2, wind.h, 100, ColorFromRGB(0, 0, 255));
	wind.fb.DrawCircle(600, 450, 25, ColorFromRGB(127, 127, 127));

	// some lines
	wind.fb.DrawLine(0, 0, 50, 100, ColorFromRGB(255, 0, 255));
	wind.fb.DrawLine(50, 100, wind.w - 50, wind.h - 10, ColorFromRGB(0, 255, 127));

	// some triangles
	wind.fb.DrawTriangle(350, 50, 350, 150, 600, 300, ColorFromRGB(127, 127, 0));
	wind.fb.DrawTriangle(50, 240, 150, 450, 300, 400, ColorFromRGB(0, 127, 127));
}
