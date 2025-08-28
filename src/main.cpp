
#include "window.hpp"
#include "color.hpp"

int main(void) {
	Window wind = Window(800, 600, "Test", 30);

	while(!wind.ShouldClose()) {
		wind.HandleEvents();
		wind.Clear(ColorFromRGB(255, 0, 0));
		wind.UpdateDisplayAndWait();
	}

	return 0;
}
