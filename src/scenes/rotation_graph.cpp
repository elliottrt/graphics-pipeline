#include "scenes/rotation_graph.hpp"
#include "color.hpp"
#include "font.hpp"
#include "math/v3.hpp"

#include <sstream>

RotationGraphScene::RotationGraphScene(Window &wind) {
	wind.Clear(0);

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

void RotationGraphScene::Update(Window &wind) {
	(void) wind;
	if (!stillGraphing) return;

	rotatedPoint = point.RotateAroundAxis(origin, axis, degrees);
	degrees += 2.f;

}

void RotationGraphScene::Render(Window &wind) {
	if (!stillGraphing) return;

	// note: we don't clear the window because we want to keep the previous graph stuff

	if (!drawnLegend) {
		DrawLegend(wind);
		drawnLegend = true;
	}

	PlotPoint(wind, rotatedPoint.x(), ColorFromRGB(255, 0, 0));
	PlotPoint(wind, rotatedPoint.y(), ColorFromRGB(0, 255, 0));
	PlotPoint(wind, rotatedPoint.z(), ColorFromRGB(0, 0, 255));

	pointIndex += 1;
	if (pointIndex * pixelScale >= wind.w) {
		stillGraphing = false;
	}

}

void RotationGraphScene::PlotPoint(Window &wind, const float &value, uint32_t color) {
	wind.DrawRect(
		pointIndex * pixelScale,
		wind.h / 2.f + ((int)(value * valueScale) * pixelScale),
		pixelScale,
		pixelScale,
		color
	);
}

void RotationGraphScene::DrawLegend(Window &wind) {
	std::stringstream ss;

	constexpr int textScale = 3;
	constexpr int padding = 8;
	constexpr int offset = textScale * FontSize() + padding;
	constexpr uint32_t WHITE = ColorFromRGB(255, 255, 255);
	
	// draw point text
	ss << "point = ";
	ss << point; // we do this to get a char* from the V3
	wind.DrawString(0, offset * 0, textScale, ss.str().c_str(), WHITE);
	ss.str(""); // reset the string stream

	// draw origin text
	ss << "origin = ";
	ss << origin;
	wind.DrawString(0, offset * 1, textScale, ss.str().c_str(), WHITE);
	ss.str("");

	// draw axis text
	ss << "axis = ";
	ss << axis;
	wind.DrawString(0, offset * 2, textScale, ss.str().c_str(), WHITE);
	ss.str("");

	// draw graph lines

	// 0 degree line
	wind.DrawLine(0, 100, 0, wind.h - 100, WHITE);
	wind.DrawString(0, wind.h - 100, textScale, "0", WHITE);

	// 360 degree line
	wind.DrawLine(360, 100, 360, wind.h - 100, WHITE);
	wind.DrawString(360, wind.h - 100, textScale, "360", WHITE);
}
