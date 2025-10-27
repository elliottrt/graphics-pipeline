#include "scenes/rotation_graph.hpp"
#include "color.hpp"
#include "font.hpp"
#include "math/v3.hpp"
#include "scene.hpp"

#include <sstream>

RotationGraphScene::RotationGraphScene(WindowGroup &g):
	Scene(g), wind(g.AddWindow(640, 480, "rotation-graph-scene"))
{
	wind->fb.Clear(0);

	pointIndex = 0;
	degrees = 0;
	valueScale = 20;
	pixelScale = 2;
	stillGraphing = true;
	drawnLegend = false;
	point = V3(2, 0.5, 1);
	axis = V3(1, 2, 3);
	origin = V3();
}

void RotationGraphScene::Update() {
	if (!stillGraphing) return;

	rotatedPoint = point.RotateAroundAxis(origin, axis, degrees);
	degrees += 2.f;

}

void RotationGraphScene::Render() {
	if (!stillGraphing) return;

	// note: we don't clear the window because we want to keep the previous graph stuff

	if (!drawnLegend) {
		DrawLegend();
		drawnLegend = true;
	}

	PlotPoint(rotatedPoint.x(), ColorFromRGB(255, 0, 0));
	PlotPoint(rotatedPoint.y(), ColorFromRGB(0, 255, 0));
	PlotPoint(rotatedPoint.z(), ColorFromRGB(0, 0, 255));

	pointIndex += 1;
	if (pointIndex * pixelScale >= wind->w) {
		stillGraphing = false;
	}

}

void RotationGraphScene::PlotPoint(const float &value, uint32_t color) {
	wind->fb.DrawRect(
		pointIndex * pixelScale,
		(int)(wind->h / 2.f + value * valueScale * pixelScale),
		pixelScale,
		pixelScale,
		color
	);
}

void RotationGraphScene::DrawLegend() {
	std::stringstream ss;

	constexpr int textScale = 3;
	constexpr int padding = 8;
	constexpr int offset = textScale * FontSize() + padding;
	constexpr uint32_t WHITE = ColorFromRGB(255, 255, 255);
	
	// draw point text
	ss << "point = ";
	ss << point; // we do this to get a char* from the V3
	wind->fb.DrawString(0, offset * 0, textScale, ss.str().c_str(), WHITE);
	ss.str(""); // reset the string stream

	// draw origin text
	ss << "origin = ";
	ss << origin;
	wind->fb.DrawString(0, offset * 1, textScale, ss.str().c_str(), WHITE);
	ss.str("");

	// draw axis text
	ss << "axis = ";
	ss << axis;
	wind->fb.DrawString(0, offset * 2, textScale, ss.str().c_str(), WHITE);
	ss.str("");

	// draw graph lines

	// 0 degree line
	wind->fb.DrawLine(0, 100, 0, wind->h - 100, WHITE);
	wind->fb.DrawString(0, wind->h - 100, textScale, "0", WHITE);

	// 360 degree line
	wind->fb.DrawLine(360, 100, 360, wind->h - 100, WHITE);
	wind->fb.DrawString(360, wind->h - 100, textScale, "360", WHITE);
}
