#include "window.hpp"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_render.h"
#include "gl.hpp"

// we only use assert for truly fatal errors, like memory allocation failure
#include <cassert>
// for informational printf
#include <cstdio>
// for abort in CheckError
#include <cstdlib>
#include <cstring>

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

#include "SDL3/SDL_video.h"
#include "frame_buffer.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

static void ImGuiSetup() {
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
	(void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

	/* TODO: does this make things better?
    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
	*/
}

static void ImGuiTearDown() {
	// TODO: I really should do this, but I can't be bothered right now
	// ImGui_ImplSDLRenderer3_Shutdown();
    // ImGui_ImplSDL3_Shutdown();
    // ImGui::DestroyContext();
}

static void ImGuiFrameStart() {
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

static void ImGuiFrameEnd(SDL_Renderer *renderer) {
	ImGuiIO &io = ImGui::GetIO();
	ImGui::Render();
	SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, 0);
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

static bool _isSetup = false;

static void SDL3TearDown() {
	// clean up SDL
	SDL_Quit();
}

static void SDL3Setup() {
	if (_isSetup) return;
	_isSetup = true;

	ImGuiSetup();

	// initialize sdl3
	assert(SDL_Init(SDL_INIT_VIDEO) == true && "SDL_Init failed");

	atexit(SDL3TearDown);
	atexit(ImGuiTearDown);
}

Window::Window(unsigned width, unsigned height, const char *title, bool useHardware):
	w(width), h(height), fb(width, height), shouldClose(false), claimedForImGui(false),
	window(NULL), renderer(NULL), texture(NULL), useHardware(useHardware), glContext(NULL) {	

	if (!_isSetup) {
		SDL3Setup();
	}

	// set up all of the components we need
	window = SDL_CreateWindow(title, w, h, 0);
	assert(window != NULL && "SDL_CreateWindow failed");

	if (useHardware) {
		glContext = SDL_GL_CreateContext(window);
		assert(glContext != NULL && "SDL_GL_CreateContext failed");

		glViewport(0, 0, width, height);
	} else {
		renderer = SDL_CreateRenderer(window, NULL);
		assert(renderer != NULL && "SDL_CreateRenderer failed");
	}

	id = SDL_GetWindowID(window);

	Resize(width, height);

}

Window::~Window() {
	// destroy all of our resources
	SDL_DestroyTexture(texture);
	if (useHardware) {
		SDL_GL_DestroyContext(glContext);
	} else {
		SDL_DestroyRenderer(renderer);
	}
	SDL_DestroyWindow(window);
}

void Window::ClaimForImGui(void) {
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
	claimedForImGui = true;
}

void Window::Resize(unsigned width, unsigned height) {
	w = width;
	h = height;

	if (useHardware) {
		glViewport(0, 0, width, height);
	} else {
		// create the new texture and delete the old one
		if (texture) SDL_DestroyTexture(texture);
		// SDL_PIXELFORMAT_ABGR8888 is the same as the in-class example
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
		printf("%s\n", SDL_GetError());
		assert(texture != NULL && "SDL_CreateTexture failed");
	}

	fb.Resize(width, height);
}

void Window::MoveTo(int x, int y) {
	SDL_SetWindowPosition(window, (int) x, (int) y);
}

void Window::SetTitle(const char *title) {
	SDL_SetWindowTitle(window, title);
}

void Window::FrameStart() {
	if (claimedForImGui) {
		ImGuiFrameStart();
	}
}

void Window::FrameEnd() {
	if (useHardware) {
		SDL_GL_SwapWindow(window);
	} else {
		// put pixels on the texture
		SDL_UpdateTexture(texture, NULL, fb.cb, w * sizeof(*fb.cb));

		// put the texture on the screen
		SDL_RenderClear(renderer);
		SDL_RenderTexture(renderer, texture, NULL, NULL);
		if (claimedForImGui) ImGuiFrameEnd(renderer);
		SDL_RenderPresent(renderer);
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
