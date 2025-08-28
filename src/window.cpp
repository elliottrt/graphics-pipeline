#include "window.hpp"

// we only use assert for truly fatal errors, like memory allocation failure
#include <OpenGL/gltypes.h>
#include <cassert>
// for informational printf
#include <cstdio>
// for sleeping to control frame rate
#include <thread>
// for abort in CheckError
#include <cstdlib>

#include <SDL3/SDL.h>

Window::Window(unsigned width, unsigned height, const char *title, unsigned fps): fb(NULL), shouldClose(false) {
	Resize(width, height);

	// initialize sdl3
	assert(SDL_Init(SDL_INIT_VIDEO) == true && "SDL_Init failed");

	// set up all of the components we need
	window = SDL_CreateWindow(title, w, h, 0);
	assert(window != NULL && "SDL_CreateWindow failed");

	renderer = SDL_CreateRenderer(window, NULL);
	assert(renderer != NULL && "SDL_CreateRenderer failed");

	// SDL_PIXELFORMAT_ABGR8888 is the same as the in-class example
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
	assert(texture != NULL && "SDL_CreateTexture failed");

	// do it this way because we never need fps itself, only its inverse
	// fps = 0 means no frame rate limit
	targetFrameTimeMs = fps != 0 ? 1000.0 / fps : 0.0;
}

Window::~Window() {
	// clean up SDL
	SDL_Quit();
	// free the framebuffer
	if (fb) delete[] fb;
}

void Window::Resize(unsigned width, unsigned height) {
	w = width;
	h = height;

	// free the old one if it existed
	if (fb) delete[] fb;

	// create the new framebuffer. undefined contents
	fb = new uint32_t [w*h];
	assert(fb != NULL && "frame buffer allocation failed");
}

void Window::HandleEvents() {
	// do some preparation for timing frame rate
	lastFrameTimeMs = SDL_GetTicks();

	// handle all of the events
	SDL_Event event;
	while (SDL_PollEvent(&event)) switch (event.type) {
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
		case SDL_EVENT_QUIT:
			shouldClose = true;
			break;
		case SDL_EVENT_WINDOW_RESIZED:
			Resize(event.window.data1, event.window.data2);
			break;
		default:
			break;
	}
}

void Window::UpdateDisplayAndWait() {

	// put pixels on the texture
	SDL_UpdateTexture(texture , NULL, fb, w * sizeof (uint32_t));

	// put the texture on the screen
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, texture , NULL, NULL);
	SDL_RenderPresent(renderer);
	
	// if we finished the frame early, wait a bit to try to hit target fps
	double frameDurationMs = SDL_GetTicks() - lastFrameTimeMs;

	if (frameDurationMs < targetFrameTimeMs) {
		std::this_thread::sleep_for(
			std::chrono::duration<double, std::milli>(targetFrameTimeMs - frameDurationMs)
		);
	}
}

void Window::SetPixel(unsigned u, unsigned v, uint32_t color) {
#ifdef WINDOW_SAFE
	if (u < w && v < h) {
#endif

	fb[u + v * w] = color;

#ifdef WINDOW_SAFE
	} else {
		printf("warning: SetPixel %ux%u out of window %ux%u\n", u, v, w, h);
	}
#endif

}

void Window::Clear(uint32_t color) {
	for (size_t uv = 0; uv < w * h; uv++) {
		fb[uv] = color;
	}
}

void Window::DrawRect(unsigned u, unsigned v, unsigned width, unsigned height, uint32_t color) {
	for (unsigned y = v; y < v + height; y++) {
		for (unsigned x = u; x < u + width; x++) {
			SetPixel(x, y, color);
		}
	}
}
