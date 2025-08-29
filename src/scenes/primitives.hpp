#ifndef SCENE_PRIMITIVES_HPP
#define SCENE_PRIMITIVES_HPP

#include "window.hpp"
#include "scene.hpp"

struct PrimitivesScene: public Scene {
	PrimitivesScene(Window &wind);

	void Update(Window &wind) override;
	void Render(Window &wind) override;
};

#endif
