#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <cstdint>
#include <SDL3/SDL.h>

struct Window {

	// width and height of both the window and framebuffer
	unsigned w;
	unsigned h;

	// milliseconds seconds per frame
	double targetFrameTimeMs;

	// frame buffer pointer
	uint32_t *fb;

	// are we done with the window?
	bool shouldClose;

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

	// basic drawing functionality
	void SetPixel(unsigned u, unsigned v, uint32_t color);
	void Clear(uint32_t color);

	// more advanced drawing
	void DrawRect(unsigned u, unsigned v, unsigned width, unsigned height, uint32_t color);

private:
	// what time did the last frame start?
	uint64_t lastFrameTimeMs;

	// sdl specific details
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
};

#endif // WINDOW_HPP