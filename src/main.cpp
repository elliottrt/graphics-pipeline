
#include "window.hpp"
#include "color.hpp"
#include "scene.hpp"

#include "scenes/pong.hpp"
#include "scenes/primitives.hpp"
#include "scenes/scrolling_name.hpp"
#include "scenes/tetris.hpp"

int main(void) {
	Window wind = Window(800, 600, "Test", 30);

	Scene *scene = new PrimitivesScene(wind);

	while(!wind.shouldClose) {
		wind.HandleEvents();

		scene->Update(wind);
		scene->Render(wind);

		wind.UpdateDisplayAndWait();
	}

	return 0;
}
