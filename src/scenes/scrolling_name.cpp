#include "scrolling_name.hpp"
#include "color.hpp"
#include "font.hpp"

#include <cstring>

ScrollingNamesScene::ScrollingNamesScene(Window &wind, const char *name) {
	this->name = name;
	textScale = 6;
	scrollSpeed = 360;
	color = ColorFromRGB(255, 0, 0);
	textPosition = wind.w;
}

void ScrollingNamesScene::Update(Window &wind) {
	textPosition -= scrollSpeed * wind.deltaTime;

	// jump back to the other side of the screen, but offscreen so we can scroll in
	int resetPoint = -(int)(strlen(name) * FontSize() * textScale);
	if (textPosition <= resetPoint) {
		textPosition = wind.w;
	}
}

void ScrollingNamesScene::Render(Window &wind) {
	wind.Clear(ColorFromRGB(0, 0, 0));
	wind.DrawString(
		(int) textPosition,
		wind.h / 2 - FontSize() * textScale / 2,
		textScale,
		name,
		color
	);
}
