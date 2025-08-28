#include "window.hpp"

// we only use assert for truly fatal errors, like memory allocation failure
#include <assert.h>
// for informational printf
#include <stdio.h>
// for sleeping to control frame rate
#include <thread>

#include <GLFW/glfw3.h>
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>

Window::Window(unsigned width, unsigned height, const char *title, unsigned fps): fps(fps), fb(NULL) {
	Resize(width, height);
	
	// try to initialize. this is vital, failure means
	// we simply can't open a window
	assert(glfwInit() == GLFW_TRUE && "glfw initialization failed");

	// glfw version 4.1
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	// profiling enabled
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create the actual window
	windowImpl = glfwCreateWindow(w, h, title, NULL, NULL);
	assert(windowImpl != NULL && "glfw window creation failed");
	glfwMakeContextCurrent((GLFWwindow *) windowImpl);

	// print out some useful information
	printf("renderer: %s\n", glGetString(GL_RENDERER));
	printf("ogl supported version: %s\n", glGetString(GL_VERSION));
	printf("monitor name: %s\n", glfwGetMonitorName(glfwGetPrimaryMonitor()));
}

Window::~Window() {
	// clean up glfw when the window object is destroyed
	glfwTerminate();
	// free the framebuffer
	if (fb) delete fb;
}

void Window::Resize(unsigned width, unsigned height) {
	w = width;
	h = height;

	// free the old one if it existed
	if (fb) delete fb;

	// create the new framebuffer. undefined contents
	fb = new uint32_t [w*h];
	assert(fb != NULL && "frame buffer allocation failed");
}

bool Window::ShouldClose() const {
	return glfwWindowShouldClose((GLFWwindow*) windowImpl);
}

void Window::HandleEvents() {
	// do some preparation for timing frame rate
	lastFrameTime = glfwGetTime();

	// glfw makes this very simple
	glfwPollEvents();

	// also close if the user presses escape
	if (glfwGetKey((GLFWwindow*) windowImpl, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose((GLFWwindow*) windowImpl, true);
}

void Window::UpdateDisplayAndWait() {
	// draw the framebuffer to the screen
	glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, fb);
	
	// if we finished the frame early, wait a bit to try to hit target fps
	double frameDuration = glfwGetTime() - lastFrameTime;

	if (frameDuration < 1.0 / fps) {
		std::this_thread::sleep_for(std::chrono::duration<double>(1.0 / fps - frameDuration));
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
