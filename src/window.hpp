#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <cstdint>

struct Window {

	// width and height of both the window and framebuffer
	unsigned w;
	unsigned h;

	// target frames per second
	unsigned fps;

	// frame buffer pointer
	uint32_t *fb;

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

private:
	// implementation defined internal value
	void *windowImpl;
	// what time did the last frame start?
	double lastFrameTime;
};

#endif // WINDOW_HPP