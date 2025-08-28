
#include "window.hpp"
#include "color.hpp"
#include "scene.hpp"

#include "scenes/pong.hpp"
#include "scenes/scrolling_name.hpp"

int main(void) {
	Window wind = Window(800, 600, "Test", 30);

	// TODO: integrate imgui?

	Scene *scene = new ScrollingNamesScene(wind, "Reed Elliott");

	while(!wind.shouldClose) {
		wind.HandleEvents();

		scene->Update(wind);
		scene->Render(wind);

		wind.UpdateDisplayAndWait();
	}

	return 0;
}
