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

	// auditorium, no textures here
	meshes[0].Load("geometry/auditorium.bin");
	meshes[0].RotateAroundAxis(meshes[0].GetCenter(), V3(1, 0, 0), 90.0f);
	meshes[0].RotateAroundAxis(meshes[0].GetCenter(), V3(0, 0, 1), 180.0f);
	meshes[0].Scale(5);

	// TODO: find better pictures - do i need to take them myself? probably

	meshes[1].LoadPlane(V3(8, -2, -49), V3(30, 0, 30), V3());
	texes[0].LoadFromTiff("geometry/bali.tif");

	meshes[2].LoadPlane(V3(1, 0, 0), V3(1, 0, 1), V3());
	texes[1].LoadFromTiff("geometry/board.tif");

	meshes[3].LoadPlane(V3(0, 1, 0), V3(1, 0, 1), V3());
	texes[2].LoadFromTiff("geometry/bali.tif");

	meshes[4].LoadPlane(V3(0, 0, 1), V3(1, 0, 1), V3());
	texes[3].LoadFromTiff("geometry/frames/frame_0.tif");

	for (size_t i = 1; i < 5; i++) {
		meshes[i].RotateAroundAxis(meshes[i].GetCenter(), V3(1, 0, 0), 90.0f);
	}

	wind.MoveTo(100, 100);
	g.AddWindow(512, 256 + 128, "gui-window", true)
		->MoveTo(200 + wind.w, 100);
}

void TextureDemoScene::Update(Window &wind) {

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

	// this is the auditorium, no need for texture because it's already colored
	meshes[0].DrawFilledNoLighting(wind.fb, camera);

	// draw the textured objects
	for (size_t i = 1; i < 5; i++) {
		meshes[i].DrawTextured(wind.fb, camera, texes[i - 1], filterMode, tilingMode);
	}

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(wind.w/2.0f, wind.h/2.0f), ImGuiCond_FirstUseEver);
	// default should be collapsed
	ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);

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
