
#include "window.hpp"
#include "color.hpp"

int main(void) {
	Window wind = Window(800, 600, "Test", 30);

	while(!wind.shouldClose) {
		wind.HandleEvents();

		wind.Clear(ColorFromRGB(255, 0, 0));
		wind.DrawRect(100, 100, 200, 150, ColorFromRGB(0, 255, 0));
		wind.DrawCircle(400, 450, 50, ColorFromRGB(0, 0, 255));
		wind.DrawLine(0, 0, 799, 599, ColorFromRGB(255, 255, 255));

		wind.UpdateDisplayAndWait();
	}

	return 0;
}
