#include "window.hpp"

// we only use assert for truly fatal errors, like memory allocation failure
#include <assert.h>
// for informational printf
#include <stdio.h>

#include <GLFW/glfw3.h>
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>

Window::Window(unsigned width, unsigned height, const char *title) {
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
