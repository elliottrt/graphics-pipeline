#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "frame_buffer.hpp"
#include <SDL3/SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

struct Window {
	friend struct WindowGroup;

	// width and height of both the window and framebuffer
	int w;
	int h;

	float deltaTime, frameTime;

	FrameBuffer fb;

	// are we done with the window?
	bool shouldClose;

	Window(unsigned width, unsigned height, const char *title);
	~Window();

	// modify the window's size
	void Resize(unsigned width, unsigned height);

	// set the window's position
	void MoveTo(int x, int y);

	// set the window's title
	void SetTitle(const char *title);

	void FrameStart();
	void FrameEnd();

	// get user keypresses
	// key is an SDL_SCANCODE_* enum value
	bool KeyPressed(int key);

private:

	// claim this window for the imgui stuff
	void ClaimForImGui(void);

	// is this the imgui window?
	bool claimedForImGui;

	// sdl specific details
	SDL_WindowID id;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
};

#endif // WINDOW_HPP