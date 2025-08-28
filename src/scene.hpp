#ifndef SCENE_HPP
#define SCENE_HPP

#include "window.hpp"

struct Scene {
	Scene(Window &) {};
	virtual void Update(Window &wind) = 0;
	virtual void Render(Window &wind) = 0;
};

#endif // SCENE_HPP
