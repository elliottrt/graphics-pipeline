#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <cstdint>

struct Window {

	// width and height of both the window and framebuffer
	unsigned w;
	unsigned h;

	// frame buffer pointer
	uint32_t *fb;

	Window(unsigned width, unsigned height, const char *title);
	~Window();

	// modify the window's size
	void Resize(unsigned width, unsigned height);

private:
	// implementation defined internal value
	void *windowImpl;
};

#endif // WINDOW_HPP