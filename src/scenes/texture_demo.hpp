#ifndef SCENE_TEXTURE_DEMO_HPP
#define SCENE_TEXTURE_DEMO_HPP

#include "frame_buffer.hpp"
#include "mesh.hpp"
#include "window.hpp"
#include "scene.hpp"
#include "ppcamera.hpp"
#include <memory>
#include <vector>

struct TextureDemoScene: public Scene {

	std::shared_ptr<Window> wind;
	PPCamera camera;
	Mesh texturedMeshes[4];
	FrameBuffer texes[4];

	enum: int {
		TILING_REPEAT = 0,
		TILING_MIRROR = 1,
	} tilingMode;
	enum: int {
		FILTER_NEAREST = 0,
		FILTER_BILINEAR = 1,
	} filterMode;
	float timer;
	unsigned frame;

	TextureDemoScene(WindowGroup &group);

	void Update() override;
	void Render() override;

};

#endif // SCENE_TEXTURE_DEMO_HPP