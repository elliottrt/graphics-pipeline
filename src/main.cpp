
#include "window.hpp"
#include "color.hpp"
#include "scene.hpp"
#include "math/v3.hpp"
#include "math/m3.hpp"

#include "scenes/pong.hpp"
#include "scenes/primitives.hpp"
#include "scenes/scrolling_name.hpp"
#include "scenes/tetris.hpp"
#include "scenes/rotation_graph.hpp"

#include <iostream>

int main(void) {
	Window wind = Window(640, 480, "Test", 30);

	// primitives: PrimitivesScene
	// scrolling name: ScrollingNamesScene
	// pong: PongGame
	// tetris: TetrisScene
	// rotation graph: RotationGraphScene
	Scene *scene = new RotationGraphScene(wind);

	while(!wind.shouldClose) {
		wind.HandleEvents();

		scene->Update(wind);
		scene->Render(wind);

		wind.UpdateDisplayAndWait();
	}
	wind.SaveToTiff("../GRAPH.tiff");

	return 0;
}
