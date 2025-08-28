
#include "window.hpp"
#include "color.hpp"
#include "scene.hpp"

#include "scenes/pong.hpp"

int main(void) {
	Window wind = Window(800, 600, "Test", 30);

	// TODO: integrate imgui?

	Scene *scene = new PongGame(wind);

	while(!wind.shouldClose) {
		wind.HandleEvents();

		scene->Update(wind);
		scene->Render(wind);

		wind.UpdateDisplayAndWait();
	}

	return 0;
}
