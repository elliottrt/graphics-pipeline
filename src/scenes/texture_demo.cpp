#include "texture_demo.hpp"
#include "color.hpp"
#include "imgui.h"
#include "mesh.hpp"
#include "ppcamera.hpp"
#include "scene.hpp"

TextureDemoScene::TextureDemoScene(WindowGroup &g, Window &wind):
	Scene(g),
	camera(wind.w, wind.h, 60.0f),
	texes{}
{

	tilingMode = TILING_REPEAT;
	filterMode = FILTER_NEAREST;
	timer = 0.0f;

	texturedMeshes[0].LoadPlane(V3(8, -2, -49), V3(30, 0, 30), V3());
	texes[0].LoadFromTiff("geometry/waterfall.tif");

	texturedMeshes[1].LoadPlane(V3(20, -2, -10), V3(6, 0, 8), V3());
	texes[1].LoadFromTiff("geometry/cat.tif");

	texturedMeshes[2].LoadPlane(V3(-10, 0, -20), V3(32, 0, 32), V3());
	texes[2].LoadFromTiff("geometry/tree_closeup.tif");
	// make this one affected by tiling mode
	for (size_t i = 0; i < 2 * texturedMeshes[2].vertexCount; i++)
		texturedMeshes[2].tcs[i] *= 3.0f; 

	texturedMeshes[3].LoadPlane(V3(40, -12, -44), V3(15, 0, 15), V3());
	texes[3].LoadFromTiff("geometry/frames/frame_0.tif");

	for (size_t i = 0; i < 4; i++) {
		texturedMeshes[i].RotateAroundDirection(V3(1, 0, 0), 90.0f);
	}

	texturedMeshes[0].RotateAroundDirection(V3(0, 0, 1), 90.0f);
	texturedMeshes[1].RotateAroundDirection(V3(0, 1, 0), 90.0f);
	texturedMeshes[2].RotateAroundDirection(V3(0, 1, 0), -90.0f);

	wind.MoveTo(100, 100);
	g.AddWindow(512, 256 + 128, "gui-window", true)
		->MoveTo(200 + wind.w, 100);
}

void TextureDemoScene::Update(Window &wind) {
	timer += wind.deltaTime;

	if (timer > 0.25f) {
		frame = (frame + 1) % 17;
		std::string path = "geometry/frames/frame_" + std::to_string(frame) + ".tif";
		texes[3].LoadFromTiff(path.c_str());
		timer = 0.0f;
	}

	constexpr static float speed = 20.0f;

	bool useGlobal = wind.KeyPressed(SDL_SCANCODE_G);

	V3 movement;
	movement.x() = (float)wind.KeyPressed(SDL_SCANCODE_D) - (float)wind.KeyPressed(SDL_SCANCODE_A);
	movement.y() = (float)wind.KeyPressed(SDL_SCANCODE_SPACE) - (float)(wind.KeyPressed(SDL_SCANCODE_LSHIFT) || wind.KeyPressed(SDL_SCANCODE_RSHIFT));
	movement.z() = (float)wind.KeyPressed(SDL_SCANCODE_S) - (float)wind.KeyPressed(SDL_SCANCODE_W);

	if (useGlobal) 
		camera.TranslateGlobal(movement * wind.deltaTime * speed);
	else
		camera.TranslateLocal(movement * wind.deltaTime * speed);

	// rotation

	V3 rotation;
	rotation.x() = (float)wind.KeyPressed(SDL_SCANCODE_UP) - (float)wind.KeyPressed(SDL_SCANCODE_DOWN);
	rotation.y() = (float)wind.KeyPressed(SDL_SCANCODE_LEFT) - (float)wind.KeyPressed(SDL_SCANCODE_RIGHT);

	if (rotation.x() != 0.0f) camera.Tilt(rotation.x() * wind.deltaTime * 45);
	if (rotation.y() != 0.0f) camera.Pan(rotation.y() * wind.deltaTime * 45);

	int zoom = (int)wind.KeyPressed(SDL_SCANCODE_EQUALS) - (int)wind.KeyPressed(SDL_SCANCODE_MINUS);

	if (zoom > 0) camera.Zoom(1 + 0.1f * wind.deltaTime);
	if (zoom < 0) camera.Zoom(1 / (1 + 0.1f * wind.deltaTime));
}

void TextureDemoScene::Render(Window &wind) {

	wind.fb.Clear(0);

	// draw the textured objects
	for (size_t i = 0; i < 4; i++) {
		texturedMeshes[i].DrawTextured(wind.fb, camera, texes[i], filterMode, tilingMode);
	}

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(wind.w/2.0f, wind.h/2.0f), ImGuiCond_FirstUseEver);
	// default should be not collapsed
	ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("debug-gui", nullptr, 0)) {
        ImGui::End();
        return;
    }

	ImGui::Text("dt: %.3f, ft: %.3f, fps: %.1f\n", wind.deltaTime, wind.frameTime, 1.0 / wind.deltaTime);

	if (ImGui::Button("reset camera")) {
		camera = PPCamera(camera.w, camera.h, camera.hfov);
	}

	static const char *tileModeNames[] = {"repeat", "mirror"};
	ImGui::ListBox("tiling mode", (int*) &tilingMode, tileModeNames, sizeof(tileModeNames) / sizeof(*tileModeNames));

	static const char *filterModeNames[] = {"nearest neighbor", "bilinear"};
	ImGui::ListBox("filter mode", (int*) &filterMode, filterModeNames, sizeof(filterModeNames) / sizeof(*filterModeNames));

	ImGui::End();

}
