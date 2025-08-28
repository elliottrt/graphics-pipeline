#include "window.hpp"

// we only use assert for truly fatal errors, like memory allocation failure
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

void Window::SetPixel(int u, int v, uint32_t color) {
#ifdef WINDOW_SAFE
	if (u < w && v < h && u >= 0 && v >= 0) {
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

void Window::DrawRect(int u, int v, unsigned width, unsigned height, uint32_t color) {
	int uMax = u + width;
	int vMax = v + height;

	// clip offscreen parts
	if (u < 0) u = 0;
	if (uMax >= (int) w) uMax = w - 1;
	if (v < 0) v = 0;
	if (vMax >= (int) h) vMax = h - 1;

	for (int y = v; y < vMax; y++) {
		for (int x = u; x < uMax; x++) {
			SetPixel(x, y, color);
		}
	}
}

void Window::DrawCircle(int u, int v, unsigned radius, uint32_t color) {
	int uMin = u - radius;
	int uMax = u + radius;
	int vMin = v - radius;
	int vMax = v + radius;

	// clip offscreen parts
	if (uMin < 0) uMin = 0;
	if (uMax >= (int) w) uMax = w - 1;
	if (vMin < 0) vMin = 0;
	if (vMax >= (int) h) vMax = h - 1;

	// very similar to DrawRect, but with a distance check against the radius
	int squareRadius = radius * radius;

	for (int y = vMin; y <= vMax; y++) {
		// we can precompute this for the inner loop
		// instead of repeatedly doing this for each value of x
		int dy = y - v;
		int squareDy = dy * dy;
		for (int x = uMin; x <= uMax; x++) {
			int dx = x - u;
			
			if (dx * dx + squareDy <= squareRadius) {
				SetPixel(x, y, color);
			}
		}
	}
}

void Window::DrawLine(int u0, int v0, int u1, int v1, uint32_t color) {
	// TODO: clip offscreen parts

	double distanceU = u1 - u0;
	double distanceV = v1 - v0;

	double steps = std::max(std::abs(distanceU), std::abs(distanceV));
	double du = distanceU / steps;
	double dv = distanceV / steps;

	for (unsigned step = 0; step <= steps; step++) {
		SetPixel(u0 + step * du, v0 + step * dv, color);
	}
}
