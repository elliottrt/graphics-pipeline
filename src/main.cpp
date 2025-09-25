
#include "window.hpp"
#include "scene.hpp"
#include "ppcamera.hpp"

#include "scenes/pong.hpp"
#include "scenes/primitives.hpp"
#include "scenes/scrolling_name.hpp"
#include "scenes/tetris.hpp"
#include "scenes/rotation_graph.hpp"
#include "scenes/camera_demo.hpp"
#include "scenes/mesh_lighting.hpp"

int main(void) {
	auto wind = Window(640, 480, "Test", 30);
	auto ppc = PPCamera(wind.w, wind.h, 60.f);

	// primitives: PrimitivesScene
	// scrolling name: ScrollingNamesScene
	// pong: PongGame
	// tetris: TetrisScene
	// rotation graph: RotationGraphScene
	// camera demo: CameraDemoScene
	// meshes and lighting: MeshLightingScene
	Scene *scene = new MeshLightingScene(wind);

	while(!wind.shouldClose) {
		wind.HandleEvents();

		scene->Update(wind);
		scene->Render(wind);

		wind.UpdateDisplayAndWait();
	}

	return 0;
}
