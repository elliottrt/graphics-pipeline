#include "camera_demo.hpp"
#include "SDL3/SDL_scancode.h"
#include "math/v3.hpp"
#include "ppcamera.hpp"

#include <utility>

CameraDemoScene::CameraDemoScene(Window &wind): camera(wind.w, wind.h, 60.f), drawnCamera(wind.w, wind.h, 60.f) {
	meshes[0].Load("geometry/teapot1K.bin");
	meshes[0].TranslateTo(V3(0, 0, -100));

	meshes[1].LoadRectangle(V3(), V3(5, 5, 5), V3(1, 1, 0));
	meshes[1].TranslateTo(V3(20, 0, 20));

	drawnCamera.TranslateGlobal(V3(5, -5, -10));

	pathProgress = 0;
	pathPlaying = false;

	pathFiles.emplace_back("geometry/path-01.txt");
	pathFiles.emplace_back("geometry/path-02.txt");
	pathFiles.emplace_back("geometry/path-03.txt");
	pathFiles.emplace_back("geometry/path-04.txt");

	camera.TranslateGlobal(V3(0, 0, 10));
}

void CameraDemoScene::Update(Window &wind) {
	meshes[0].RotateAroundAxis(meshes[0].GetCenter(), V3(0.0f, 1.0f, 0.0f), 90.f * wind.deltaTime);
	meshes[2].LoadAABB(meshes[0].GetAABB(), V3(1, 0, 0));

	// translation

	bool useGlobal = wind.KeyPressed(SDL_SCANCODE_G);

	V3 movement;
	movement.x() = (int)wind.KeyPressed(SDL_SCANCODE_D) - (int)wind.KeyPressed(SDL_SCANCODE_A);
	movement.y() = (int)wind.KeyPressed(SDL_SCANCODE_SPACE) - (int)(wind.KeyPressed(SDL_SCANCODE_LSHIFT) || wind.KeyPressed(SDL_SCANCODE_RSHIFT));
	movement.z() = (int)wind.KeyPressed(SDL_SCANCODE_S) - (int)wind.KeyPressed(SDL_SCANCODE_W);

	if (useGlobal) 
		camera.TranslateGlobal(movement * wind.deltaTime * 10);
	else
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

	int zoom = (int)wind.KeyPressed(SDL_SCANCODE_EQUALS) - (int)wind.KeyPressed(SDL_SCANCODE_MINUS);

	if (zoom > 0) camera.Zoom(1 + 0.1 * wind.deltaTime);
	if (zoom < 0) camera.Zoom(1 / (1 + 0.1 * wind.deltaTime));

	if (wind.KeyPressed(SDL_SCANCODE_C))
		camera.SaveToFile("camera.txt");
	if (wind.KeyPressed(SDL_SCANCODE_V))
		camera.LoadFromFile("camera.txt");

	if (wind.KeyPressed(SDL_SCANCODE_P) && !pathPlaying) {
		pathPlaying = true;
		pathFrame = 0;
	}

	if (pathPlaying) {
		pathProgress += wind.deltaTime * (2.90 / 10.0);
		SetCameraOnPath();
	}
}

void CameraDemoScene::Render(Window &wind) {
	wind.Clear(0);

	meshes[0].DrawWireframe(wind, camera);
	meshes[1].DrawWireframe(wind, camera);
	meshes[2].DrawWireframe(wind, camera);

	wind.DrawCamera(camera, drawnCamera);

	if (pathPlaying) {
		char filepath[32];
		snprintf(filepath, sizeof(filepath), "image-%03zu.tiff", pathFrame++);
		wind.SaveToTiff(filepath);
	}
}

void CameraDemoScene::SetCameraOnPath(void) {
	if (!pathPlaying) return;

	if (pathProgress >= pathFiles.size() - 1) {
		pathPlaying = false;
		pathProgress = 0;
		return;
	}

	size_t phase = (size_t) pathProgress;

	PPCamera start(camera.w, camera.h, camera.hfov); start.LoadFromFile(pathFiles[phase]);
	PPCamera end(camera.w, camera.h, camera.hfov); end.LoadFromFile(pathFiles[phase + 1]);

	camera = start.InterpolateSmooth(end, pathProgress - phase);
}
