#ifndef SCENE_TEXTURE_DEMO_HPP
#define SCENE_TEXTURE_DEMO_HPP

#include "frame_buffer.hpp"
#include "mesh.hpp"
#include "window.hpp"
#include "scene.hpp"
#include "ppcamera.hpp"
#include <vector>

struct TextureDemoScene: public Scene {

	PPCamera camera;
	Mesh mesh;
	FrameBuffer tex;

	TextureDemoScene(WindowGroup &group, Window &wind);

	void Update(Window &wind) override;
	void Render(Window &wind) override;

};

#endif // SCENE_TEXTURE_DEMO_HPP