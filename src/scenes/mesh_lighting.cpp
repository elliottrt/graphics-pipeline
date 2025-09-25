#include "scenes/mesh_lighting.hpp"
#include "imgui.h"
#include "math/v3.hpp"
#include "mesh_scene.hpp"
#include "ppcamera.hpp"

#include <memory>

MeshLightingScene::MeshLightingScene(Window &wind): MeshScene(wind), camera(wind.w, wind.h, 60.f) {
	// create a teapot mesh
	meshes.push_back(std::make_unique<Mesh>());
	meshes.back()->Load("geometry/teapot1K.bin");
	meshes.back()->TranslateTo(V3(0, 0, -100));

	lightPosition = V3(5, 5, -20);

	// disable imgui.ini stuff
	ImGui::GetIO().IniFilename = NULL;
}

void MeshLightingScene::Update() {
	meshes[0]->RotateAroundAxis(meshes[0]->GetCenter(), V3(0.0f, 1.0f, 0.0f), 15.f * wind.deltaTime);

	// translation

	bool useGlobal = wind.KeyPressed(SDL_SCANCODE_G);

	V3 movement;
	movement.x() = (float)wind.KeyPressed(SDL_SCANCODE_D) - (float)wind.KeyPressed(SDL_SCANCODE_A);
	movement.y() = (float)wind.KeyPressed(SDL_SCANCODE_SPACE) - (float)(wind.KeyPressed(SDL_SCANCODE_LSHIFT) || wind.KeyPressed(SDL_SCANCODE_RSHIFT));
	movement.z() = (float)wind.KeyPressed(SDL_SCANCODE_S) - (float)wind.KeyPressed(SDL_SCANCODE_W);

	if (useGlobal) 
		camera.TranslateGlobal(movement * wind.deltaTime * 10);
	else
		camera.TranslateLocal(movement * wind.deltaTime * 10);

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

void MeshLightingScene::Render() {
	wind.Clear(0);

	wind.DrawPoint(camera, lightPosition, 7, V3(1, 1, 1));

	for (const auto &m : meshes) {
		m->DrawFilled(wind, camera);
	}

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(wind.w, wind.h), ImGuiCond_FirstUseEver);
	// default should be collapsed
	ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("debug-gui", nullptr, 0)) {
        ImGui::End();
        return;
    }

	if (ImGui::Button("reset gui")) {
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImVec2(550, 680));
		ImGui::SetWindowCollapsed(true);
	}

	if (ImGui::Button("reset camera")) {
		camera = PPCamera(camera.w, camera.h, camera.hfov);
	}

	ImGui::DragFloat3("light pos", lightPosition);

	ImGui::End();
	// ImGui::ShowDemoWindow();
}
