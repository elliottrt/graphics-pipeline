#ifndef SCENE_HPP
#define SCENE_HPP

#include "window.hpp"
#include "window_group.hpp"

struct Scene {
	Scene(WindowGroup &) {}; // do nothing, but require a group in the constructor
	virtual void Update(Window &wind) = 0;
	virtual void Render(Window &wind) = 0;
};

#endif // SCENE_HPP
