#include "hardware_demo.hpp"

#include "color.hpp"
#include "font.hpp"
#include "gl.hpp"
#include "ppcamera.hpp"
#include <OpenGL/gl.h>
#include <fstream>
#include <iostream>

HardwareDemoScene::HardwareDemoScene(WindowGroup &g):
	Scene(g), wind(g.AddWindow(1280, 720, "hardware-demo-scene", false, true)),
	camera(wind->w, wind->h, 60.0f), fill(true), cameraSaveKeyDown(false)
{
	// filledTexMesh.Load("geometry/teapot1K.bin");
	filledTexMesh.Load2DPlane(50, 50);
	filledTexMesh.TranslateTo(V3(-50.0f, -25.0f, -300.0f));

	// wireTexMesh.Load("geometry/teapot1K.bin");
	wireTexMesh.Load2DPlane(50, 50);
	wireTexMesh.TranslateTo(V3(50.0f, -25.0f, -300.0f));

	filledColorMesh.Load("geometry/teapot1K.bin");
	filledColorMesh.TranslateTo(V3(-50.0f, 0, -150.0f));

	wireColorMesh.Load("geometry/teapot1K.bin");
	wireColorMesh.TranslateTo(V3(50.0f, 0, -150.0f));

	floorMesh.LoadPlane(V3(0, -25.0f, -100.0f), V3(200, 0, 200), V3(1, 0, 1));

	uiMesh.Load2DPlane(wind->w, wind->h);

	tex.LoadFromTiff("geometry/uffizi_front.tiff");
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	hwTexFromFb(texId, tex);

	glGenTextures(1, &uiTex);
	glBindTexture(GL_TEXTURE_2D, uiTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	hwInit();
}

void HardwareDemoScene::Update(void) {
	constexpr static float speed = 20.0f;
	constexpr static float cameraPlaySpeed = 0.5f;

	if (playingPath) {
		pathProgress += wind->deltaTime * cameraPlaySpeed;

		if (pathProgress >= 1.0f) {
			pathProgress -= 1.0f;
			pathStream >> frameStartCamera;
			if (pathStream.eof()) {
				playingPath = false;
				pathStream.str(std::string());
				pathProgress = 0.0f;
				camera = frameEndCamera;
				return;
			}
			std::swap(frameStartCamera, frameEndCamera);
		}

		camera = frameStartCamera.InterpolateSmooth(frameEndCamera, pathProgress - (long)pathProgress);
	} else {
		bool useGlobal = wind->KeyPressed(SDL_SCANCODE_G);

		V3 movement;
		movement.x() = (float)wind->KeyPressed(SDL_SCANCODE_D) - (float)wind->KeyPressed(SDL_SCANCODE_A);
		movement.y() = (float)wind->KeyPressed(SDL_SCANCODE_SPACE) - (float)(wind->KeyPressed(SDL_SCANCODE_LSHIFT) || wind->KeyPressed(SDL_SCANCODE_RSHIFT));
		movement.z() = (float)wind->KeyPressed(SDL_SCANCODE_S) - (float)wind->KeyPressed(SDL_SCANCODE_W);

		if (useGlobal) 
			camera.TranslateGlobal(movement * wind->deltaTime * speed);
		else
			camera.TranslateLocal(movement * wind->deltaTime * speed);

		// rotation

		V3 rotation;
		rotation.x() = (float)wind->KeyPressed(SDL_SCANCODE_UP) - (float)wind->KeyPressed(SDL_SCANCODE_DOWN);
		rotation.y() = (float)wind->KeyPressed(SDL_SCANCODE_LEFT) - (float)wind->KeyPressed(SDL_SCANCODE_RIGHT);

		if (rotation.x() != 0.0f) camera.Tilt(rotation.x() * wind->deltaTime * 45);
		if (rotation.y() != 0.0f) camera.Pan(rotation.y() * wind->deltaTime * 45);
	}

	camera.SetGLView();

	if (wind->KeyPressed(SDL_SCANCODE_I)) {
		if (!cameraSaveKeyDown) {
			std::ofstream ofs("camera_path.txt", std::ios::app | std::ios::out);
			if (ofs) {
				ofs << camera << std::endl;
				ofs.close();
			}
			cameraSaveKeyDown = true;
		}
	} else {
		cameraSaveKeyDown = false;
	}

	if (wind->KeyPressed(SDL_SCANCODE_P)) {
		if (!cameraPlayPathKeyDown && !playingPath) {
			playingPath = true;
			pathProgress = 0.0f;
			cameraPlayPathKeyDown = true;

			std::ifstream ifs("camera_path.txt");
			if (ifs) {
				pathStream.clear();
				pathStream << ifs.rdbuf();
				pathStream >> frameStartCamera;
				pathStream >> frameEndCamera;
				camera = frameStartCamera;
				camera.SetGLView();
     			ifs.close();
			} else {
				std::cerr << "error: unable to open camera path file" << std::endl;
			}
		}
	} else {
		cameraPlayPathKeyDown = false;
	}
}

void HardwareDemoScene::Render(void) {
	camera.InitializeGL(0.1f, 1000.0f);
	camera.SetGLView();

	hwClear(V3());
	hwDrawMesh(filledTexMesh, true, texId);
	hwDrawMesh(wireTexMesh, false, texId);
	hwDrawMesh(filledColorMesh, true);
	hwDrawMesh(wireColorMesh, false);
	hwDrawMesh(floorMesh, true);

	constexpr static int scale = 3;

	std::string fpsString = std::to_string(wind->frameTime);
	wind->fb.DrawRect(0, 0, scale * FontSize() * fpsString.size(), scale * FontSize(), 0);
	wind->fb.DrawString(0, 0, 3, fpsString.c_str(), ColorFromRGB(255, 255, 255));
	hwTexFromFb(uiTex, wind->fb);

	hwBegin2D(wind->w, wind->h);
	hwDrawMesh(uiMesh, true, uiTex);
	hwEnd2D();
}
