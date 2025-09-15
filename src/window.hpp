#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "ppcamera.hpp"
#include <cstdint>
#include <SDL3/SDL.h>

struct Window {

	// width and height of both the window and framebuffer
	int w;
	int h;

	// milliseconds seconds per frame
	double targetFrameTimeMs;

	// frame buffer pointer
	uint32_t *fb;

	// are we done with the window?
	bool shouldClose;

	// delta time for consistent updates
	double deltaTime;

	Window(unsigned width, unsigned height, const char *title, unsigned fps);
	~Window();

	// modify the window's size
	void Resize(unsigned width, unsigned height);

	// for loop control over events and drawing
	bool ShouldClose() const;

	// deal with window events
	void HandleEvents();

	// show the framebuffer to the user
	// also controls frame rate
	void UpdateDisplayAndWait();

	// get user keypresses
	// key is an SDL_SCANCODE_* enum value
	bool KeyPressed(int key);

	// TIFF file IO
	// copied and modified from framebuffer.cpp example code
	bool SaveToTiff(const char *path) const;
	bool LoadFromTiff(const char *path);

	// basic drawing functionality
	void SetPixel(int u, int v, uint32_t color);
	void Clear(uint32_t color);

	// more advanced drawing
	void DrawRect(int u, int v, unsigned width, unsigned height, uint32_t color);
	void DrawCircle(int u, int v, unsigned radius, uint32_t color);
	void DrawLine(int u0, int v0, int u1, int v1, uint32_t color);
	void DrawLine(int u0, int v0, int u1, int v1, const V3 &c0, const V3 &c1);
	void DrawTriangle(int u0, int v0, int u1, int v1, int u2, int v2, uint32_t color);
	void DrawChar(int u, int v, unsigned scale, char ch, uint32_t color);
	void DrawString(int u, int v, unsigned scale, const char *string, uint32_t color);

	// 3d drawing
	void DrawPoint(const PPCamera &camera, const V3 &point, size_t pointSize, uint32_t color);
	void DrawCamera(const PPCamera &camera, const PPCamera &drawnCamera);

private:
	// what time did the last frame start?
	uint64_t lastFrameTimeMs;

	// sdl specific details
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
};

#endif // WINDOW_HPP