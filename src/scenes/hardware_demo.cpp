#include "hardware_demo.hpp"

#include "color.hpp"
#include "font.hpp"
#include "gl.hpp"
#include "ppcamera.hpp"
#include <OpenGL/gl.h>
#include <iostream>

HardwareDemoScene::HardwareDemoScene(WindowGroup &g):
	Scene(g), wind(g.AddWindow(1280, 720, "hardware-demo-scene", false, true)),
	camera(wind->w, wind->h, 60.0f), fill(true)
{
	filledTexMesh.Load("geometry/teapot1K.bin");
	filledTexMesh.TranslateTo(V3(-50.0f, 0, -150.0f));

	wireTexMesh.Load("geometry/teapot1K.bin");
	wireTexMesh.TranslateTo(V3(50.0f, 0, -150.0f));

	filledColorMesh.Load("geometry/teapot1K.bin");
	filledColorMesh.TranslateTo(V3(-50.0f, 0, -300.0f));

	wireColorMesh.Load("geometry/teapot1K.bin");
	wireColorMesh.TranslateTo(V3(50.0f, 0, -300.0f));

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

	camera.SetGLView();
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
