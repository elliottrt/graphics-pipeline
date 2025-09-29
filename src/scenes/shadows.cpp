#include "scenes/shadows.hpp"
#include "imgui.h"
#include "math/v3.hpp"

ShadowScene::ShadowScene(Window &wind):
	userCamera(wind.w, wind.h, 60.0f), lightCamera(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 90.0f),
	lightBuffer(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE) {

	ground.LoadRectangle(V3(0, -15.5, -100), V3(100, 1, 100), V3(0.5, 0.5, 0.5));
	caster.Load("geometry/teapot1K.bin");
	caster.TranslateTo(V3(0, 15, -100));

	lightCamera.LoadFromString("0.620327 2.03346e-08 -0.784344 0.632368 -0.591591 0.500132 -439.477 -54.9495 -21.1884 43.2141 63.6061 -60.2703 512 512");

}

void ShadowScene::Update(Window &wind) {
	bool useGlobal = wind.KeyPressed(SDL_SCANCODE_G);

	V3 movement;
	movement.x() = (float)wind.KeyPressed(SDL_SCANCODE_D) - (float)wind.KeyPressed(SDL_SCANCODE_A);
	movement.y() = (float)wind.KeyPressed(SDL_SCANCODE_SPACE) - (float)(wind.KeyPressed(SDL_SCANCODE_LSHIFT) || wind.KeyPressed(SDL_SCANCODE_RSHIFT));
	movement.z() = (float)wind.KeyPressed(SDL_SCANCODE_S) - (float)wind.KeyPressed(SDL_SCANCODE_W);

	if (useGlobal) 
		userCamera.TranslateGlobal(movement * wind.deltaTime * 10);
	else
		userCamera.TranslateLocal(movement * wind.deltaTime * 10);

	// rotation

	V3 rotation;
	rotation.x() = (float)wind.KeyPressed(SDL_SCANCODE_UP) - (float)wind.KeyPressed(SDL_SCANCODE_DOWN);
	rotation.y() = (float)wind.KeyPressed(SDL_SCANCODE_LEFT) - (float)wind.KeyPressed(SDL_SCANCODE_RIGHT);

	if (rotation.x() != 0.0f) userCamera.Tilt(rotation.x() * wind.deltaTime * 45);
	if (rotation.y() != 0.0f) userCamera.Pan(rotation.y() * wind.deltaTime * 45);

	int zoom = (int)wind.KeyPressed(SDL_SCANCODE_EQUALS) - (int)wind.KeyPressed(SDL_SCANCODE_MINUS);

	if (zoom > 0) userCamera.Zoom(1 + 0.1f * wind.deltaTime);
	if (zoom < 0) userCamera.Zoom(1 / (1 + 0.1f * wind.deltaTime));
}

void ShadowScene::Render(Window &wind) {
	wind.fb.Clear(0);
	lightBuffer.Clear(0);

	// draw everything for the shadow map first
	ground.DrawFilledNoLighting(lightBuffer, lightCamera);
	caster.DrawFilledNoLighting(lightBuffer, lightCamera);

	// wind.fb.DrawZBuffer(lightBuffer);

	// TODO: draw the lit scene for the user
	ground.DrawFilledNoLighting(wind.fb, userCamera);
	caster.DrawFilledNoLighting(wind.fb, userCamera);
	wind.fb.DrawCamera(userCamera, lightCamera);

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(wind.w/2.0f, wind.h/2.0f), ImGuiCond_FirstUseEver);
	// default should be collapsed
	ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("debug-gui", nullptr, 0)) {
        ImGui::End();
        return;
    }

	ImGui::Text("dt: %.3f, fps: %.1f", wind.deltaTime, 1.0 / wind.deltaTime);

	if (ImGui::Button("reset camera")) {
		userCamera = PPCamera(userCamera.w, userCamera.h, userCamera.hfov);
	}

	/*
	// used for positioning the light
	if (ImGui::Button("swap cameras")) {
		std::swap(userCamera, lightCamera);
	}

	if (ImGui::Button("dump light camera")) {
		lightCamera.SaveToFile("lightCamera.txt");
	}
	*/

	ImGui::End();
}
