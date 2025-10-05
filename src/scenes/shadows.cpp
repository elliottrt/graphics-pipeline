#include "scenes/shadows.hpp"
#include "imgui.h"
#include "math/v3.hpp"
#include "scene.hpp"

ShadowScene::ShadowScene(WindowGroup &g, Window &wind):
	Scene(g),
	userCamera(wind.w, wind.h, 60.0f),
	lightCamera(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 90.0f),
	lightWindow(g.AddWindow(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, "light-buffer"))
{

	ka = 0.4;
	specularIntensity = 100.0f;
	lookAtPoint = V3(0, 10, -100);
	teapotPosition = V3(10, 2, -90);
	teapotAngle = lastAngle = 0.0f;

	ground.LoadPlane(V3(0, -25, -150), V3(100, 1, 200), V3(0.5, 0.5, 0.5));

	caster.Load("geometry/teapot57K.bin");
	caster.TranslateTo(teapotPosition);

	lightCamera.Pose(V3(0, 50, -10), lookAtPoint, V3(0, 1, 0));
	UpdateLightBuffer();

	wind.MoveTo(100, 100);
	lightWindow->MoveTo(wind.w + 150, 100);

	auto guiWindow = g.AddWindow(512, 256 + 128, "gui-window", true);
	guiWindow->MoveTo(100, 100 + wind.h + 100);
}

void ShadowScene::Update(Window &wind) {
	if (teapotAngle != lastAngle) {
		caster.RotateAroundAxis(caster.GetCenter(), V3(0.0f, 1.0f, 0.0f), teapotAngle - lastAngle);
		lastAngle = teapotAngle;
	}

	caster.TranslateTo(teapotPosition);

	bool useGlobal = wind.KeyPressed(SDL_SCANCODE_G);

	V3 movement;
	movement.x() = (float)wind.KeyPressed(SDL_SCANCODE_D) - (float)wind.KeyPressed(SDL_SCANCODE_A);
	movement.y() = (float)wind.KeyPressed(SDL_SCANCODE_SPACE) - (float)(wind.KeyPressed(SDL_SCANCODE_LSHIFT) || wind.KeyPressed(SDL_SCANCODE_RSHIFT));
	movement.z() = (float)wind.KeyPressed(SDL_SCANCODE_S) - (float)wind.KeyPressed(SDL_SCANCODE_W);

	if (useGlobal) 
		userCamera.TranslateGlobal(movement * wind.deltaTime * 30);
	else
		userCamera.TranslateLocal(movement * wind.deltaTime * 30);

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

	ground.DrawFilledPointLight(wind.fb, userCamera, lightCamera, lightWindow->fb, ka, specularIntensity);
	caster.DrawFilledPointLight(wind.fb, userCamera, lightCamera, lightWindow->fb, ka, specularIntensity);
	wind.fb.DrawCamera(userCamera, lightCamera);

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

	bool didUpdate = false;

	didUpdate |= ImGui::DragFloat3("light position", lightCamera.C);
	didUpdate |= ImGui::DragFloat3("teapot position", teapotPosition);
	didUpdate |= ImGui::DragFloat("teapotAngle", &teapotAngle, 1.0f, -180.0f, 180.0f);

	if (didUpdate) UpdateLightBuffer();

	ImGui::End();
}

void ShadowScene::UpdateLightBuffer() {
	lightWindow->fb.Clear(0);

	ground.DrawFilledNoLighting(lightWindow->fb, lightCamera);
	caster.DrawFilledNoLighting(lightWindow->fb, lightCamera);

	lightWindow->fb.DrawZBuffer();
}
