#ifndef SCENE_ROTATION_GRAPH_HPP
#define SCENE_ROTATION_GRAPH_HPP

#include "window.hpp"
#include "scene.hpp"
#include "math/v3.hpp"

struct RotationGraphScene: public Scene {

	int pointIndex;
	float degrees;
	float valueScale;
	float pixelScale;
	bool stillGraphing;
	bool drawnLegend;
	V3 point, axis, origin, rotatedPoint;

	RotationGraphScene(Window &wind);

	void Update(Window &wind) override;
	void Render(Window &wind) override;

	void PlotPoint(Window &wind, const float &value, uint32_t color);
	void DrawLegend(Window &wind);

};

#endif
