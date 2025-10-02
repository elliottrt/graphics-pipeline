#include "scenes/shadows.hpp"
#include "imgui.h"
#include "math/v3.hpp"

ShadowScene::ShadowScene(Window &wind):
	userCamera(wind.w, wind.h, 60.0f), lightCamera(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 90.0f),
	lightBuffer(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE) {

	renderShadowMap = false;
	ka = 0.4;
	specularIntensity = 100.0f;
	lookAtPoint = V3(0, 10, -100);

	ground.LoadRectangle(V3(0, -15, -100), V3(100, 1, 100), V3(0.5, 0.5, 0.5));
	caster.Load("geometry/teapot1K.bin");
	caster.TranslateTo(V3(10, 12, -90));

	lightCamera.Pose(V3(0, 50, -10), lookAtPoint, V3(0, 1, 0));

}

void ShadowScene::Update(Window &wind) {
	bool useGlobal = wind.KeyPressed(SDL_SCANCODE_G);

	V3 movement;
	movement.x() = (float)wind.KeyPressed(SDL_SCANCODE_D) - (float)wind.KeyPressed(SDL_SCANCODE_A);
	movement.y() = (float)wind.KeyPressed(SDL_SCANCODE_SPACE) - (float)(wind.KeyPressed(SDL_SCANCODE_LSHIFT) || wind.KeyPressed(SDL_SCANCODE_RSHIFT));
	movement.z() = (float)wind.KeyPressed(SDL_SCANCODE_S) - (float)wind.KeyPressed(SDL_SCANCODE_W);

	if (useGlobal) 
		userCamera.TranslateGlobal(movement * wind.deltaTime * 20);
	else
		userCamera.TranslateLocal(movement * wind.deltaTime * 20);

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
	// TODO: we only need to do this when the light or scene changes
	UpdateLightBuffer();

	ground.DrawFilledPointLight(wind.fb, userCamera, lightCamera, lightBuffer, ka, specularIntensity);
	caster.DrawFilledPointLight(wind.fb, userCamera, lightCamera, lightBuffer, ka, specularIntensity);
	wind.fb.DrawCamera(userCamera, lightCamera);

	if (renderShadowMap) {
		//lightBuffer.DrawPointCloud(lightCamera, wind.fb, userCamera);
		//wind.fb.Copy(lightBuffer);

		wind.fb.DrawZBuffer(lightBuffer);
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
		userCamera = PPCamera(userCamera.w, userCamera.h, userCamera.hfov);
	}

	if (ImGui::Button("move camera to light")) {
		userCamera.Pose(lightCamera.C, lookAtPoint, V3(0, 1, 0));
	}

	ImGui::Checkbox("render shadowmap", &renderShadowMap);

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

void ShadowScene::UpdateLightBuffer() {
	ground.DrawFilledNoLighting(lightBuffer, lightCamera);
	caster.DrawFilledNoLighting(lightBuffer, lightCamera);
}
