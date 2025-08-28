
#include "window.hpp"
#include "color.hpp"

int main(void) {
	Window wind = Window(800, 600, "Test", 30);

	while(!wind.shouldClose) {
		wind.HandleEvents();

		wind.Clear(ColorFromRGB(255, 0, 0));
		wind.DrawRect(100, 100, 200, 150, ColorFromRGB(0, 255, 0));

		wind.UpdateDisplayAndWait();
	}

	return 0;
}
