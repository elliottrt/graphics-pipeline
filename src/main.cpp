
#include "window.hpp"
#include "color.hpp"
#include "scene.hpp"
#include "math/v3.hpp"
#include "math/m3.hpp"
#include "ppcamera.hpp"

#include "scenes/pong.hpp"
#include "scenes/primitives.hpp"
#include "scenes/scrolling_name.hpp"
#include "scenes/tetris.hpp"
#include "scenes/rotation_graph.hpp"

#include <iostream>

int main(void) {
	auto wind = Window(640, 480, "Test", 30);
	auto ppc = PPCamera(wind.w, wind.h, 60.f);

	// primitives: PrimitivesScene
	// scrolling name: ScrollingNamesScene
	// pong: PongGame
	// tetris: TetrisScene
	// rotation graph: RotationGraphScene
	// Scene *scene = new RotationGraphScene(wind);

	V3 PP;
	V3 cubeCenter = V3(0, 0, -10);
	V3 cube[8] = {
		{-1,  1,  1}, // 0
		{-1, -1,  1}, // 1
		{ 1, -1,  1}, // 2
		{ 1,  1,  1}, // 3
		{-1,  1, -1}, // 4
		{-1, -1, -1}, // 5
		{ 1, -1, -1}, // 6
		{ 1,  1, -1}, // 7
	};

	while(!wind.shouldClose) {
		wind.HandleEvents();

		// scene->Update(wind);
		// scene->Render(wind);

		wind.Clear(0);
		for (int i = 0; i < 8; i++) {
			ppc.ProjectPoint(cube[i] * 2 + cubeCenter, PP);
			wind.DrawCircle(PP.x(), PP.y(), 7, ColorFromRGB(255, 255, 255));
		}

		wind.UpdateDisplayAndWait();
	}

	return 0;
}
