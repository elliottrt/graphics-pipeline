
#include "window.hpp"
#include "color.hpp"
#include "scene.hpp"
#include "math/v3.hpp"

#include "scenes/pong.hpp"
#include "scenes/primitives.hpp"
#include "scenes/scrolling_name.hpp"
#include "scenes/tetris.hpp"

int main(void) {
	Window wind = Window(640, 480, "Test", 30);

	Scene *scene = new TetrisScene(wind);

	while(!wind.shouldClose) {
		wind.HandleEvents();

		scene->Update(wind);
		scene->Render(wind);

		wind.UpdateDisplayAndWait();
	}

	return 0;
}
