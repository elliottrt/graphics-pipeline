#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "frame_buffer.hpp"
#include <SDL3/SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

struct Window {

	// width and height of both the window and framebuffer
	int w;
	int h;

	FrameBuffer fb;

	// milliseconds seconds per frame
	double targetFrameTimeMs;

	// are we done with the window?
	bool shouldClose;

	// delta time for consistent updates
	float deltaTime;

	Window(unsigned width, unsigned height, const char *title, unsigned fps);
	~Window();

	// modify the window's size
	void Resize(unsigned width, unsigned height);

	// deal with window events
	void HandleEvents();

	// show the framebuffer to the user
	// also controls frame rate
	void UpdateDisplayAndWait();

	// get user keypresses
	// key is an SDL_SCANCODE_* enum value
	bool KeyPressed(int key);

private:
	// what time did the last frame start?
	uint64_t lastFrameTimeMs;

	// sdl specific details
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
};

#endif // WINDOW_HPP