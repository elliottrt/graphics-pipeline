#include "window_group.hpp"
#include "SDL3/SDL_video.h"
#include <cassert>
#include <memory>
#include <thread>
#include <utility>

WindowGroup::WindowGroup(unsigned fps): shouldClose(false), windows() {
	
	// do it this way because we never need fps itself, only its inverse
	// fps = 0 means no frame rate limit
	targetFrameTimeMs = fps != 0 ? 1000.0 / fps : 0.0;

	hasGuiWindow = false;

}

std::shared_ptr<Window> WindowGroup::AddWindow(unsigned width, unsigned height, const char *title, bool imgui) {
	auto w = std::make_shared<Window>(width, height, title);
	SDL_ShowWindow(w.get()->window);
	auto id = w->id;
	windows[id] = w;
	if (imgui) ClaimForImgui(*w);
	return windows[id];
}

void WindowGroup::HandleEvents() {
	// do some preparation for timing frame rate
	lastFrameTimeMs = SDL_GetTicks();

	// handle all of the events
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (hasGuiWindow) ImGui_ImplSDL3_ProcessEvent(&event);

		switch (event.type) {
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			case SDL_EVENT_QUIT:
				shouldClose = true;
				break;
			case SDL_EVENT_WINDOW_RESIZED:
				windows
					.at(event.window.windowID)
					->Resize(event.window.data1, event.window.data2);
				break;
			default:
				break;
		}
	}

	for (auto &w : windows) {
		w.second->FrameStart();
	}
}

void WindowGroup::UpdateAndWait() {

	for (auto &w : windows) {
		w.second->FrameEnd();
	}

	// if we finished the frame early, wait a bit to try to hit target fps
	double frameDurationMs = (double)(SDL_GetTicks() - lastFrameTimeMs);
	frameTime = frameDurationMs / 1000.0f;

	if (frameDurationMs < targetFrameTimeMs) {
		std::this_thread::sleep_for(
			std::chrono::duration<double, std::milli>(targetFrameTimeMs - frameDurationMs)
		);

		deltaTime = (float) targetFrameTimeMs / 1000.0f;
	} else {
		deltaTime = (float) frameDurationMs / 1000.0f;
	}

	for (auto &w : windows) {
		w.second->deltaTime = deltaTime;
		w.second->frameTime = frameTime;
	}
}

void WindowGroup::ClaimForImgui(Window &wind) {
	assert(windows.count(wind.id) > 0 && "window must be in the window group");
	assert(hasGuiWindow == false && "cannot have multiple GUI windows");
	wind.ClaimForImGui();
	hasGuiWindow = true;
}
