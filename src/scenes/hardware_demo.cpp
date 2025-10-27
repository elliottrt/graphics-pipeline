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

	camera.InitializeGL(0.1f, 1000.0f);
	camera.SetGLView();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void HardwareDemoScene::Update(void) {

}

void HardwareDemoScene::Render(void) {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	constexpr bool fill = false;

	if (fill) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// enable stuff
	if (mesh.vertices) glEnableClientState(GL_VERTEX_ARRAY);
	if (mesh.normals) glEnableClientState(GL_NORMAL_ARRAY);
	if (mesh.colors) glEnableClientState(GL_COLOR_ARRAY);
	if (mesh.tcs) glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// load our pointers
	if (mesh.vertices) glVertexPointer(3, GL_FLOAT, 0, mesh.vertices);
	if (mesh.normals) glNormalPointer(GL_FLOAT, 0, mesh.normals);
	if (mesh.colors) glColorPointer(3, GL_FLOAT, 0, mesh.colors);
	if (mesh.tcs) glTexCoordPointer(2, GL_FLOAT, 0, mesh.tcs);

	// draw stuff
	glDrawElements(GL_TRIANGLES, 3 * mesh.triangleCount, GL_UNSIGNED_INT, mesh.triangles);

	// disable stuff
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
