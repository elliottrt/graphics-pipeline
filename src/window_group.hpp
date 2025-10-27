#ifndef WINDOW_GROUP_HPP
#define WINDOW_GROUP_HPP

#include "window.hpp"
#include <memory>
#include <unordered_map>

struct WindowGroup {

	// delta time for consistent updates
	float deltaTime, frameTime;
	
	bool shouldClose;

	WindowGroup(unsigned fps);

	std::shared_ptr<Window> AddWindow(
		unsigned width,
		unsigned height,
		const char *title,
		bool imgui = false,
		bool useHardware = false
	);

	// handle each event, dispatching them to their relevant windows
	void HandleEvents();

	// update the displays of each window and wait to achieve target fps
	void UpdateAndWait();

	// set a window to the imgui target
	void ClaimForImgui(Window &wind);

private:
	std::unordered_map<SDL_WindowID, std::shared_ptr<Window>> windows;
	double targetFrameTimeMs;
	// what time did the last frame start?
	uint64_t lastFrameTimeMs;

	bool hasGuiWindow;
};

#endif // WINDOW_GROUP_HPP