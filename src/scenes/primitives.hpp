#ifndef SCENE_PRIMITIVES_HPP
#define SCENE_PRIMITIVES_HPP

#include "window.hpp"
#include "scene.hpp"
#include <memory>

struct PrimitivesScene: public Scene {
	std::shared_ptr<Window> wind;

	PrimitivesScene(WindowGroup &group);

	void Update() override;
	void Render() override;
};

#endif
