#include "hardware_demo.hpp"

#include "SDL3/SDL_video.h"
#include "gl.hpp"
#include "ppcamera.hpp"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <iostream>

HardwareDemoScene::HardwareDemoScene(WindowGroup &g):
	Scene(g), wind(g.AddWindow(1280, 720, "hardware-demo-scene", false, true)),
	camera(wind->w, wind->h, 60.0f)
{
	mesh.Load("geometry/teapot1K.bin");
	mesh.TranslateTo(V3(0, 0, -100.0f));

	// mesh.LoadPlane(V3(0, 0, -100.0f), V3(50, 0, 50), V3(1, 1, 1));
	// mesh.RotateAroundDirection(V3(1, 0, 0), -90);

	tex.LoadFromTiff("geometry/uffizi_front.tiff");
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.w, tex.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.cb);
	// glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	camera.InitializeGL(0.1f, 1000.0f);
	camera.SetGLView();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
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
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	constexpr bool fill = true;

	if (fill) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// load our pointers
	if (mesh.vertices) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, mesh.vertices);
	}
	if (mesh.normals) {
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, mesh.normals);
	}
	if (mesh.colors) {
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, mesh.colors);
	}
	if (mesh.tcs) {
		glBindTexture(GL_TEXTURE_2D, texId);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, mesh.tcs);
	}

	// draw stuff
	glDrawElements(GL_TRIANGLES, 3 * mesh.triangleCount, GL_UNSIGNED_INT, mesh.triangles);

	// disable stuff
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
