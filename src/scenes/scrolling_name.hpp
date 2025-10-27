#ifndef SCENES_SCROLLING_NAME_HPP
#define SCENES_SCROLLING_NAME_HPP

#include "window.hpp"
#include "scene.hpp"
#include <memory>

struct ScrollingNamesScene: public Scene {

	std::shared_ptr<Window> wind;
	const char *name;
	unsigned textScale;
	double scrollSpeed;
	uint32_t color;
	double textPosition;

	ScrollingNamesScene(WindowGroup &group);

	void Update() override;
	void Render() override;

};

#endif // SCENES_SCROLLING_NAME_HPP
