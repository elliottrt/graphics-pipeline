#include "camera_demo.hpp"
#include "SDL3/SDL_scancode.h"
#include "color.hpp"

#include <iostream>
#include <utility>

CameraDemoScene::CameraDemoScene(Window &wind): camera(wind.w, wind.h, 60.f), drawnCamera(wind.w, wind.h, 60.f) {
	teapot.Load(RES_DIR"/geometry/teapot1K.bin");
	teapot.TranslateTo(V3(0, 0, -100));

	drawnCamera.TranslateGlobal(V3(5, -5, -10));

	camera.TranslateGlobal(V3(0, 0, 10));
}

void CameraDemoScene::Update(Window &wind) {
	teapot.RotateAroundAxis(teapot.GetCenter(), V3(0.0f, 1.0f, 0.0f), 90.f * wind.deltaTime);

	// translation

	V3 movement;
	movement.z() = (int)wind.KeyPressed(SDL_SCANCODE_W) - (int)wind.KeyPressed(SDL_SCANCODE_S);
	movement.x() = (int)wind.KeyPressed(SDL_SCANCODE_D) - (int)wind.KeyPressed(SDL_SCANCODE_A);
	movement.y() = (int)wind.KeyPressed(SDL_SCANCODE_SPACE) - (int)(wind.KeyPressed(SDL_SCANCODE_LSHIFT) || wind.KeyPressed(SDL_SCANCODE_RSHIFT));

	camera.TranslateLocal(movement * wind.deltaTime * 10);

	// rotation

	V3 rotation;
	rotation.x() = (int)wind.KeyPressed(SDL_SCANCODE_UP) - (int)wind.KeyPressed(SDL_SCANCODE_DOWN);
	rotation.y() = (int)wind.KeyPressed(SDL_SCANCODE_LEFT) - (int)wind.KeyPressed(SDL_SCANCODE_RIGHT);

	if (rotation.x() != 0.0f) camera.Tilt(rotation.x() * wind.deltaTime * 45);
	if (rotation.y() != 0.0f) camera.Pan(rotation.y() * wind.deltaTime * 45);

	if (wind.KeyPressed(SDL_SCANCODE_F)) {
		std::swap(camera, drawnCamera);
	}
}

void CameraDemoScene::Render(Window &wind) {
	wind.Clear(0);
	// teapot.DrawVertices(wind, camera, 5);
	teapot.DrawWireframe(wind, camera);

	wind.DrawCamera(camera, drawnCamera);
}
