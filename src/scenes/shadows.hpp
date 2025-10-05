#ifndef SHADOWS_SCENE
#define SHADOWS_SCENE

#include "scene.hpp"
#include "window.hpp"
#include "ppcamera.hpp"
#include "mesh.hpp"

constexpr int SHADOW_MAP_SIZE = 512;

struct ShadowScene: public Scene {

	PPCamera userCamera;

	PPCamera lightCamera;
	std::shared_ptr<Window> lightWindow;
	float ka;
	float specularIntensity;
	V3 lookAtPoint;

	Mesh ground, caster;
	V3 teapotPosition;
	float teapotAngle, lastAngle;

	ShadowScene(WindowGroup &group, Window &wind);

	void Update(Window &wind) override;
	void Render(Window &wind) override;

	void UpdateLightBuffer();

};

#endif
