
#include "mesh.hpp"
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
#include "scenes/shadows.hpp"

int main(void) {
	auto wind = Window(640, 480, "graphics-pipeline", 30);
	wind.ClaimForImGui();

	// TODO: get multiple windows working
	// auto wind2 = Window(512, 512, "light-buffer", 30);

	// primitives: PrimitivesScene
	// scrolling name: ScrollingNamesScene
	// pong: PongGame
	// tetris: TetrisScene
	// rotation graph: RotationGraphScene
	// camera demo: CameraDemoScene
	// meshes and lighting: MeshLightingScene
	// shadows from a single light: ShadowScene
	auto *scene = new ShadowScene(wind);

	while(!wind.shouldClose) {
		wind.HandleEvents();

		scene->Update(wind);
		scene->Render(wind);

		wind.UpdateDisplayAndWait();
	}

	return 0;
}
