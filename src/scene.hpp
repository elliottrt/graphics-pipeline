#ifndef SCENE_HPP
#define SCENE_HPP

#include "window.hpp"
#include "window_group.hpp"

struct Scene {
	Scene(WindowGroup &) {}; // do nothing, but require a group in the constructor
	virtual void Update(void) = 0;
	virtual void Render(void) = 0;
};

#endif // SCENE_HPP
