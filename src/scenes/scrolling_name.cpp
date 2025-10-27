#include "scrolling_name.hpp"
#include "color.hpp"
#include "font.hpp"
#include "scene.hpp"

#include <cstring>

ScrollingNamesScene::ScrollingNamesScene(WindowGroup &g):
	Scene(g), wind(g.AddWindow(640, 480, "scrolling-name-scene"))
{
	this->name = "Reed Elliott";
	textScale = 6;
	scrollSpeed = 360;
	color = ColorFromRGB(255, 0, 0);
	textPosition = wind->w;
}

void ScrollingNamesScene::Update() {
	textPosition -= scrollSpeed * wind->deltaTime;

	// jump back to the other side of the screen, but offscreen so we can scroll in
	int resetPoint = -(int)(strlen(name) * FontSize() * textScale);
	if (textPosition <= resetPoint) {
		textPosition = wind->w;
	}
}

void ScrollingNamesScene::Render() {
	wind->fb.Clear(ColorFromRGB(0, 0, 0));
	wind->fb.DrawString(
		(int) textPosition,
		wind->h / 2 - FontSize() * textScale / 2,
		textScale,
		name,
		color
	);
}
