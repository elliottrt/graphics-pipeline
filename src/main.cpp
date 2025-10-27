
#include "window_group.hpp"
#include "window.hpp"

#include "scenes/pong.hpp"
#include "scenes/primitives.hpp"
#include "scenes/scrolling_name.hpp"
#include "scenes/tetris.hpp"
#include "scenes/rotation_graph.hpp"
#include "scenes/camera_demo.hpp"
#include "scenes/mesh_lighting.hpp"
#include "scenes/shadows.hpp"
#include "scenes/texture_demo.hpp"
#include "scenes/envmapping.hpp"

int main(void) {
	auto g = WindowGroup(30);

	// primitives: PrimitivesScene
	// scrolling name: ScrollingNamesScene
	// pong: PongGame
	// tetris: TetrisScene
	// rotation graph: RotationGraphScene
	// camera demo: CameraDemoScene
	// meshes and lighting: MeshLightingScene
	// shadows from a single light: ShadowScene
	// textures: TextureDemoScene
	// environment mapping: EnvironmentMappingScene
	auto *scene = new EnvironmentMappingScene(g);

	while(!g.shouldClose) {
		g.HandleEvents();

		scene->Update();
		scene->Render();

		g.UpdateAndWait();
	}

	return 0;
}
