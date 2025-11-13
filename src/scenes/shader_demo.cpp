#include "shader_demo.hpp"

#include "color.hpp"
#include "cube_map.hpp"
#include "font.hpp"
#include "frame_buffer.hpp"
#include "gl.hpp"
#include "mesh.hpp"
#include "ppcamera.hpp"
#include "shader.hpp"
#include <OpenGL/gl.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

constexpr static const size_t IMPOSTOR_SIZE = 512;

static const std::array<std::string, CubeMap::N> sides = {
	"geometry/uffizi_right.tiff",
	"geometry/uffizi_left.tiff",
	"geometry/uffizi_top.tiff",
	"geometry/uffizi_bottom.tiff",
	"geometry/uffizi_back.tiff",
	"geometry/uffizi_front.tiff"
};

ShaderDemoScene::ShaderDemoScene(WindowGroup &g):
	Scene(g), wind(g.AddWindow(1280, 720, "shader-demo-scene", false, true)),
	camera(wind->w, wind->h, 60.0f),
	shader("../geometry/envmap.vert", "../geometry/envmap.frag"),
	backgroundShader("../geometry/envmap_background.vert", "../geometry/envmap_background.frag")
{
	cube.LoadRectangle(V3(), V3(1, 1, 1), V3());

	reflectiveMesh.Load("geometry/teapot57K.bin");
	reflectiveMesh.TranslateTo(V3(0, 0, -150.0f));

	impostorMeshes[0].Load("geometry/teapot1K.bin");
	impostorMeshes[0].Scale(0.5);
	impostorMeshes[0].TranslateTo(V3(75, 0, -110));

	V3 tpBBN = impostorMeshes[0].GetCenter() - reflectiveMesh.GetCenter();
	float scale = -100;
	V3 axis0 = V3(0, scale, 0);
	V3 axis1 = tpBBN.Cross(axis0).Normalized() * scale;

	// TODO: is this correct???
	impostorV0[0] = impostorMeshes[0].GetCenter() + axis1 - axis0;
	impostorV1[0] = impostorMeshes[0].GetCenter() + axis1 + axis0;
	impostorV2[0] = impostorMeshes[0].GetCenter() - axis1 + axis0;

	// if this is too close to the reflector, it gets culled which is bad.
	impostorMeshes[1].LoadPlane(
		reflectiveMesh.GetCenter() + V3(1, -22, 0),
		V3(1, 0, 1) * 100,
		V3(1, 0, 1)
	);
	impostorV0[1] = impostorMeshes[1].vertices[1];
	impostorV1[1] = impostorMeshes[1].vertices[2];
	impostorV2[1] = impostorMeshes[1].vertices[0];
	auto floorFb = FrameBuffer::CreateChecker(IMPOSTOR_SIZE, IMPOSTOR_SIZE, 8);
	floorTex = hwCreateTexture();
	hwTexFromFb(floorTex, floorFb);

	// create impostors

	hwInit();

	FrameBuffer fbs[IMPOSTOR_COUNT];

	for (size_t i = 0; i < IMPOSTOR_COUNT; i++) {
		fbs[i] = FrameBuffer::CreateImpostor(
			IMPOSTOR_SIZE, IMPOSTOR_SIZE,
			reflectiveMesh.GetCenter(), impostorMeshes[i], i == 1 ? &floorFb : NULL
		);

		// stupid dumb opengl 2.1 struct uniforms
		char buf[128] = {0};
		snprintf(buf, sizeof(buf), "impostors[%zu].V0", i);
		lV0[i] = shader.GetUniformLocation(buf);

		snprintf(buf, sizeof(buf), "impostors[%zu].V1", i);
		lV1[i] = shader.GetUniformLocation(buf);

		snprintf(buf, sizeof(buf), "impostors[%zu].V2", i);
		lV2[i] = shader.GetUniformLocation(buf);
	}

	lTex = shader.GetUniformLocation("impostorTex");
	lEye = shader.GetUniformLocation("eye");
	lEyeBack = backgroundShader.GetUniformLocation("eye");

	auto bigFb = FrameBuffer(IMPOSTOR_COUNT * IMPOSTOR_SIZE, IMPOSTOR_SIZE);

	for (size_t i = 0; i < IMPOSTOR_COUNT; i++) {
		bigFb.Copy(fbs[i], IMPOSTOR_SIZE * i, 0);
	}

	// auto &f = floorFb;
	// g.AddWindow(f.w, f.h, "imptex")->fb = f;

	impostorTex = hwCreateTexture();
	hwTexFromFb(impostorTex, bigFb);

	// cube map stuff
	// see https://learnopengl.com/Advanced-OpenGL/Cubemaps

	CubeMap m(sides);

	cubemapLoc = shader.GetUniformLocation("cubemap");
	cubemapLocBack = backgroundShader.GetUniformLocation("cubemap");
	cubemapTex = hwCreateTexture(GL_TEXTURE_CUBE_MAP);

	for (size_t i = 0; i < CubeMap::N; i++) {
		FrameBuffer &cmfb = m.buffers[i];
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGBA, cmfb.w, cmfb.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, cmfb.cb);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
}

void ShaderDemoScene::Update(void) {
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
}

void ShaderDemoScene::Render(void) {
	cube.TranslateTo(camera.C);
	camera.InitializeGL(0.1f, 1000.0f);
	camera.SetGLView();

	hwClear(V3());

	backgroundShader.Enable();
		backgroundShader.SetUniform(cubemapLoc, cubemapTex);
		backgroundShader.SetUniform(lEyeBack, camera.C);
		glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);
		hwDrawMesh(cube, true);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
	backgroundShader.Disable();

	shader.Enable();
		shader.SetUniform(lEye, camera.C);
		shader.SetUniform(lTex, impostorTex, 1);
		for (size_t i = 0; i < IMPOSTOR_COUNT; i++) {
			shader.SetUniform(lV0[i], impostorV0[i]);
			shader.SetUniform(lV1[i], impostorV1[i]);
			shader.SetUniform(lV2[i], impostorV2[i]);
		}
		shader.SetUniform(cubemapLoc, cubemapTex);

		hwDrawMesh(reflectiveMesh, true);
	shader.Disable();

	hwDrawMesh(impostorMeshes[0], true);
	hwDrawMesh(impostorMeshes[1], true, floorTex);
}
