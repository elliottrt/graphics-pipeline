#include "window.hpp"
#include "SDL3/SDL_render.h"

// we only use assert for truly fatal errors, like memory allocation failure
#include <cassert>
// for informational printf
#include <cstdio>
// for sleeping to control frame rate
#include <thread>
// for abort in CheckError
#include <cstdlib>
#include <cstring>

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

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
	ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
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

Window::Window(unsigned width, unsigned height, const char *title, unsigned fps):
	w(width), h(height), fb(width, height), shouldClose(false), window(NULL), renderer(NULL), texture(NULL) {	

	if (!_isSetup) {
		SDL3Setup();
	}

	// set up all of the components we need
	window = SDL_CreateWindow(title, w, h, 0);
	assert(window != NULL && "SDL_CreateWindow failed");

	renderer = SDL_CreateRenderer(window, NULL);
	assert(renderer != NULL && "SDL_CreateRenderer failed");

	Resize(width, height);

	// do it this way because we never need fps itself, only its inverse
	// fps = 0 means no frame rate limit
	targetFrameTimeMs = fps != 0 ? 1000.0 / fps : 0.0;

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}

Window::~Window() {
	// destroy all of our resources
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void Window::Resize(unsigned width, unsigned height) {
	w = width;
	h = height;

	// create the new texture and delete the old one
	if (texture) SDL_DestroyTexture(texture);
	// SDL_PIXELFORMAT_ABGR8888 is the same as the in-class example
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
	printf("%s\n", SDL_GetError());
	assert(texture != NULL && "SDL_CreateTexture failed");

	fb.Resize(width, height);
}

void Window::HandleEvents() {
	// do some preparation for timing frame rate
	lastFrameTimeMs = SDL_GetTicks();

	// handle all of the events
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL3_ProcessEvent(&event);

		switch (event.type) {
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

	ImGuiFrameStart();
}

void Window::UpdateDisplayAndWait() {

	// put pixels on the texture
	SDL_UpdateTexture(texture, NULL, fb.cb, w * sizeof(*fb.cb));

	// put the texture on the screen
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, texture, NULL, NULL);
	ImGuiFrameEnd(renderer);
	SDL_RenderPresent(renderer);
	
	// if we finished the frame early, wait a bit to try to hit target fps
	double frameDurationMs = (double)(SDL_GetTicks() - lastFrameTimeMs);

	if (frameDurationMs < targetFrameTimeMs) {
		std::this_thread::sleep_for(
			std::chrono::duration<double, std::milli>(targetFrameTimeMs - frameDurationMs)
		);

		deltaTime = (float) targetFrameTimeMs / 1000.0f;
	} else {
		deltaTime = (float) frameDurationMs / 1000.0f;
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
