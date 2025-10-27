#ifndef SCENE_ROTATION_GRAPH_HPP
#define SCENE_ROTATION_GRAPH_HPP

#include "window.hpp"
#include "scene.hpp"
#include "math/v3.hpp"
#include <memory>

struct RotationGraphScene: public Scene {

	std::shared_ptr<Window> wind;
	int pointIndex;
	float degrees;
	int valueScale;
	int pixelScale;
	bool stillGraphing;
	bool drawnLegend;
	V3 point, axis, origin, rotatedPoint;

	RotationGraphScene(WindowGroup &group);

	void Update() override;
	void Render() override;

	void PlotPoint(const float &value, uint32_t color);
	void DrawLegend();

};

#endif
