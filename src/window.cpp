#include "window.hpp"
#include "SDL3/SDL_keyboard.h"
#include "font.hpp"

// we only use assert for truly fatal errors, like memory allocation failure
#include <cassert>
// for informational printf
#include <cstdio>
// for sleeping to control frame rate
#include <thread>
// for abort in CheckError
#include <cstdlib>

#include <SDL3/SDL.h>
#include <tiff.h>
#include <tiffio.h>

Window::Window(unsigned width, unsigned height, const char *title, unsigned fps):
	w(width), h(height), fb(NULL), shouldClose(false), window(NULL), renderer(NULL), texture(NULL) {

	// initialize sdl3
	assert(SDL_Init(SDL_INIT_VIDEO) == true && "SDL_Init failed");

	// set up all of the components we need
	window = SDL_CreateWindow(title, w, h, 0);
	assert(window != NULL && "SDL_CreateWindow failed");

	renderer = SDL_CreateRenderer(window, NULL);
	assert(renderer != NULL && "SDL_CreateRenderer failed");

	Resize(width, height);

	// do it this way because we never need fps itself, only its inverse
	// fps = 0 means no frame rate limit
	targetFrameTimeMs = fps != 0 ? 1000.0 / fps : 0.0;
}

Window::~Window() {
	// destroy all of our resources
	SDL_DestroyTexture(texture);
	if (fb) delete[] fb;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	// clean up SDL
	SDL_Quit();
}

void Window::Resize(unsigned width, unsigned height) {
	w = width;
	h = height;

	// free the old one if it existed
	if (fb) delete[] fb;

	// create the new texture and delete the old one
	if (texture) SDL_DestroyTexture(texture);
	// SDL_PIXELFORMAT_ABGR8888 is the same as the in-class example
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
	printf("%s\n", SDL_GetError());
	assert(texture != NULL && "SDL_CreateTexture failed");

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

	// TODO: is this really the best option?
	deltaTime = targetFrameTimeMs / 1000.0;

	if (frameDurationMs < targetFrameTimeMs) {
		std::this_thread::sleep_for(
			std::chrono::duration<double, std::milli>(targetFrameTimeMs - frameDurationMs)
		);
	}
}

bool Window::KeyPressed(int key) {
	static const bool *keyState = NULL;
	static int keyStateLength = 0;

	if (keyState == NULL)
		keyState = SDL_GetKeyboardState(&keyStateLength);

	if (key > 0 && key < keyStateLength)
		return keyState[key];
	else
	 	return false;
}

// copied and modified from framebuffer.cpp example code
bool Window::SaveToTiff(const char *path) const {
	TIFF* out = TIFFOpen(path, "w");

	if (out == NULL) {
		fprintf(stderr, "error: could not load tiff from %s\n", path);
		return false;
	}

	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	for (int row = 0; row < h; row++) {
		TIFFWriteScanline(out, &fb[row * w], row);
	}

	TIFFClose(out);
	return true;
}

// copied and modified from framebuffer.cpp example code
bool Window::LoadFromTiff(const char *path) {
	TIFF* in = TIFFOpen(path, "r");
	if (in == NULL) {
		fprintf(stderr, "error: could not read tiff from %s\n", path);
		return false;
	}

	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);

	if (w != width || h != height) {
		Resize(width, height);
	}

	if (TIFFReadRGBAImageOriented(in, w, h, fb, ORIENTATION_TOPLEFT, 0) == 0) {
		TIFFClose(in);
		fprintf(stderr, "error: could not load tiff from %s\n", path);
		return false;
	}

	TIFFClose(in);
	return true;
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
	size_t pixelCount = w * h;
	for (size_t uv = 0; uv < pixelCount; uv++) {
		fb[uv] = color;
	}
}

void Window::DrawRect(int u, int v, unsigned width, unsigned height, uint32_t color) {
	int uMax = u + width;
	int vMax = v + height;

	// clip offscreen parts
	if (u < 0) u = 0;
	if (uMax >= w) uMax = w - 1;
	if (v < 0) v = 0;
	if (vMax >= h) vMax = h - 1;

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
	if (uMax >= w) uMax = w - 1;
	if (vMin < 0) vMin = 0;
	if (vMax >= h) vMax = h - 1;

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

// similar idea to some of those in class
// walking along a path based on a direction vector
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

/*
void Window::DrawTriangle(int u0, int v0, int u1, int v1, int u2, int v2, uint32_t color) {
	// TODO: DrawTriangle
}
*/

void Window::DrawChar(int u, int v, unsigned scale, char ch, uint32_t color) {
	const uint8_t *bits = FontGetChar(ch);

	// we're using a bitmap font, so we iterate through each row
	for (unsigned row = 0; row < FontSize(); row++) {
		uint8_t byte = bits[row];

		// for each bit in the row, we fill in the rectangle if that bit is a 1
		for (unsigned col = 0; col < FontSize(); col++) {
			int bitValue = byte & 1;
			byte >>= 1;

			if (bitValue) {
				DrawRect(u, v, scale, scale, color);
			}

			u += scale;
		}
		
		u -= scale * FontSize();
		v += scale;
	}
}

void Window::DrawString(int u, int v, unsigned scale, const char *string, uint32_t color) {
	int charU = u;
	int charV = v;

	for (const char *ch = string; *ch; ch++) {
		DrawChar(charU, charV, scale, *ch, color);
		
		if (*ch == '\n') {
			charU = u;
			charV += FontSize() * scale;
		} else {
			charU += FontSize() * scale;
		}
	}
}
