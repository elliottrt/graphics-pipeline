#ifndef SHADOWS_SCENE
#define SHADOWS_SCENE

#include "frame_buffer.hpp"
#include "scene.hpp"
#include "window.hpp"
#include "ppcamera.hpp"
#include "mesh.hpp"

constexpr int SHADOW_MAP_SIZE = 512;

struct ShadowScene: public Scene {

	PPCamera userCamera;

	PPCamera lightCamera;
	FrameBuffer lightBuffer;
	float ka;
	float specularIntensity;
	V3 lookAtPoint;

	// should we render the shadow map to the main camera
	bool renderShadowMap;

	Mesh ground, caster;

	ShadowScene(Window &wind);

	void Update(Window &wind) override;
	void Render(Window &wind) override;

	void UpdateLightBuffer();

};

#endif
