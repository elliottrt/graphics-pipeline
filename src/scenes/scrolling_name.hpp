#ifndef SCENES_SCROLLING_NAME_HPP
#define SCENES_SCROLLING_NAME_HPP

#include "window.hpp"
#include "scene.hpp"

struct ScrollingNamesScene: public Scene {

	const char *name;
	unsigned textScale;
	double scrollSpeed;
	uint32_t color;
	double textPosition;

	ScrollingNamesScene(Window &wind, const char *name);

	void Update(Window &wind) override;
	void Render(Window &wind) override;

};

#endif // SCENES_SCROLLING_NAME_HPP
