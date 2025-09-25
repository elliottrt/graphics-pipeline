#include "window.hpp"
#include "SDL3/SDL_render.h"
#include "color.hpp"
#include "font.hpp"
#include "math/v3.hpp"

// we only use assert for truly fatal errors, like memory allocation failure
#include <cassert>
// for informational printf
#include <cstdio>
// for sleeping to control frame rate
#include <cstring>
#include <thread>
// for abort in CheckError
#include <cstdlib>
// min and max
#include <algorithm>

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <tiff.h>
#include <tiffio.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

static void ImGuiSetup(SDL_Window *window, SDL_Renderer *renderer) {
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

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
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

Window::Window(unsigned width, unsigned height, const char *title, unsigned fps):
	w(width), h(height), fb(NULL), zb(NULL), shouldClose(false), window(NULL), renderer(NULL), texture(NULL) {

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

	ImGuiSetup(window, renderer);
}

Window::~Window() {
	// destroy all of our resources
	SDL_DestroyTexture(texture);
	if (fb) delete[] fb;
	if (zb) delete[] zb;
	ImGuiTearDown();
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
	if (zb) delete[] zb;

	// create the new texture and delete the old one
	if (texture) SDL_DestroyTexture(texture);
	// SDL_PIXELFORMAT_ABGR8888 is the same as the in-class example
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
	printf("%s\n", SDL_GetError());
	assert(texture != NULL && "SDL_CreateTexture failed");

	// create the new framebuffer. undefined contents
	fb = new uint32_t [w*h];
	assert(fb != NULL && "frame buffer allocation failed");

	zb = new float [w * h];
	assert(zb != NULL && "z buffer allocation failed");
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
	SDL_UpdateTexture(texture, NULL, fb, w * sizeof(uint32_t));

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
	// clear color buffer
	size_t pixelCount = w * h;
	for (size_t uv = 0; uv < pixelCount; uv++) {
		fb[uv] = color;
	}

	// clear z buffer
	memset(zb, 0, pixelCount * sizeof(*zb));
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
	// TODO: clip offscreen parts faster, by adjusting steps, u, and v

	float distanceU = (float) (u1 - u0);
	float distanceV = (float) (v1 - v0);

	float steps = std::max(std::abs(distanceU), std::abs(distanceV));
	float du = distanceU / steps;
	float dv = distanceV / steps;

	float u = (float) u0, v = (float) v0;

	for (unsigned step = 0; step <= steps; step++) {
		if (u >= 0 && v >= 0 && u < w && v < h) SetPixel((int) u, (int) v, color);

		u += du;
		v += dv;
	}
}

// same as draw line, but interpolates colors
void Window::DrawLine(int u0, int v0, int u1, int v1, const V3 &c0, const V3 &c1) {
	// TODO: clip offscreen parts faster, by adjusting steps, u, and v

	float distanceU = (float) (u1 - u0);
	float distanceV = (float) (v1 - v0);

	float steps = std::max(std::abs(distanceU), std::abs(distanceV));
	float du = distanceU / steps;
	float dv = distanceV / steps;

	float u = (float) u0, v = (float) v0;

	for (unsigned step = 0; step <= steps; step++) {
		if (u >= 0 && v >= 0 && u < w && v < h)
			SetPixel((int) u, (int) v, ColorFromV3(c0.Interpolate(c1, step / steps)));

		u += du;
		v += dv;
	}
}

// implementation from Lectures/TRast.pdf
// this took me a long time. I couldn't figure out where the variable i came from
void Window::DrawTriangle(int u0, int v0, int u1, int v1, int u2, int v2, uint32_t color) {

	// line equation values
	V3 a = {
		(float) (v1 - v0), // edge 0,1
		(float) (v2 - v1), // edge 1,2
		(float) (v0 - v2), // edge 2,0
	};
	V3 b = {
		(float) (-u1 + u0), // edge 0,1
		(float) (-u2 + u1), // edge 1,2
		(float) (-u0 + u2), // edge 2,0
	};
	V3 c = {
		(float) (-u0*v1 + v0*u1), // edge 0,1
		(float) (-u1*v2 + v1*u2), // edge 1,2
		(float) (-u2*v0 + v2*u0), // edge 2,0
	};

	// sidedness calculations for each edge

	// edge 0,1
	if (a[0] * u2 + b[0] * v2 + c[0] < 0) {
		a[0] = -a[0];
		b[0] = -b[0];
		c[0] = -c[0];
	}

	// edge 1,2
	if (a[1] * u0 + b[1] * v0 + c[1] < 0) {
		a[1] = -a[1];
		b[1] = -b[1];
		c[1] = -c[1];
	}

	// edge 2,0
	if (a[2] * u1 + b[2] * v1 + c[2] < 0) {
		a[2] = -a[2];
		b[2] = -b[2];
		c[2] = -c[2];
	}

	// bounding box calculations
	float boundingBox[4] = {
		// left & right
		(float) std::min({u0, u1, u2}), (float) std::max({u0, u1, u2}),
		// top & bottom
		(float) std::min({v0, v1, v2}), (float) std::max({v0, v1, v2})
	};

	// clip box to window
	if (boundingBox[0] < 0.f) boundingBox[0] = 0.f;
	if (boundingBox[1] >= w) boundingBox[1] = (float) w;
	if (boundingBox[2] < 0.f) boundingBox[2] = 0.f;
	if (boundingBox[3] >= h) boundingBox[3] = (float) h;

	int left = (int) (boundingBox[0] + 0.5f);
	int right = (int) (boundingBox[1] - 0.5f);
	int top = (int) (boundingBox[2] + 0.5f);
	int bottom = (int) (boundingBox[3] - 0.5f);

	// edge expression values for line starts and within line
	V3 currEELS, currEE;

	for (int i = 0; i < 3; i++) {
		currEELS[i] = a[i] * (left+0.5f) + b[i] * (top+0.5f) + c[i];
	}

	for (int currPixY = top; currPixY <= bottom; currPixY++, currEELS += b) {
		currEE = currEELS;

		for (int currPixX = left; currPixX <= right; currPixX++, currEE += a) {
			if (currEE[0] >= 0 && currEE[1] >= 0 && currEE[2] >= 0) {
				SetPixel(currPixX, currPixY, color);
			}
		}
	}
}

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

void Window::SetPixel(const V3 &p, const V3 &color) {
	SetPixel((int) p.x(), (int) p.y(), p.z(), color);
}

void Window::SetPixel(int u, int v, float z, const V3 &color) {
#ifdef WINDOW_SAFE
	if (u < w && v < h && u >= 0 && v >= 0) {
#endif

	int i = u + v * w;

	if (z > zb[i]) {
		zb[i] = z;
		fb[i] = ColorFromV3(color);
	}

#ifdef WINDOW_SAFE
	} else {
		printf("warning: SetPixel %ux%u out of window %ux%u\n", u, v, w, h);
	}
#endif
}

void Window::DrawPoint(const PPCamera &camera, const V3 &point, size_t pointSize, const V3 &color) {
	V3 PP;
	float hsize = pointSize / 2.0f;
	if (camera.ProjectPoint(point, PP)) {
		for (int v = PP.y() - hsize; v < PP.y() + hsize; v++) {
			for (int u = PP.x() - hsize; u < PP.x() + hsize; u++) {
				if (u >= 0 && v >= 0 && u < w && v < h)
					SetPixel(u, v, PP.z(), color);
			}
		}
	}
}

void Window::DrawLine(const V3 &p0, const V3 &p1, const V3 &c0, const V3 &c1) {
	// TODO: clip offscreen parts faster, by adjusting steps, u, and v

	V3 D = p1 - p0;

	float steps = std::max(std::abs(D.x()), std::abs(D.y()));

	V3 iter = p0;

	for (unsigned step = 0; step <= steps; step++) {
		if (iter.x() >= 0 && iter.y() >= 0 && iter.x() < w && iter.y() < h)
			SetPixel(iter, c0.Interpolate(c1, step / steps));

		iter += (D / steps);
	}
}

void Window::DrawCamera(const PPCamera &camera, const PPCamera &drawnCamera) {
	constexpr static uint32_t WHITE = ColorFromRGB(255, 255, 255);
	constexpr static float SCALE = 5;

	DrawPoint(camera, drawnCamera.C, 7, V3(1, 1, 1));

	float f = SCALE / camera.GetFocalLength();

	V3 pC;
	bool p0 = camera.ProjectPoint(drawnCamera.C, pC);
	V3 c[4];
	bool p1 = camera.ProjectPoint(drawnCamera.C + (drawnCamera.c) * f, c[0]);
	bool p2 = camera.ProjectPoint(drawnCamera.C + (drawnCamera.c + drawnCamera.a * (float) drawnCamera.w) * f, c[1]);
	bool p3 = camera.ProjectPoint(drawnCamera.C + (drawnCamera.c + drawnCamera.a * (float) drawnCamera.w + drawnCamera.b * (float) drawnCamera.h) * f, c[2]);
	bool p4 = camera.ProjectPoint(drawnCamera.C + (drawnCamera.c + drawnCamera.b * (float) drawnCamera.h) * f, c[3]);

	if (p0 && p1) DrawLine((int)pC.x(), (int)pC.y(), (int)c[0].x(), (int)c[0].y(), WHITE);
	if (p0 && p2) DrawLine((int)pC.x(), (int)pC.y(), (int)c[1].x(), (int)c[1].y(), WHITE);
	if (p0 && p3) DrawLine((int)pC.x(), (int)pC.y(), (int)c[2].x(), (int)c[2].y(), WHITE);
	if (p0 && p4) DrawLine((int)pC.x(), (int)pC.y(), (int)c[3].x(), (int)c[3].y(), WHITE);

	if (p1 && p2) DrawLine((int)c[0].x(), (int)c[0].y(), (int)c[1].x(), (int)c[1].y(), WHITE);
	if (p2 && p3) DrawLine((int)c[1].x(), (int)c[1].y(), (int)c[2].x(), (int)c[2].y(), WHITE);
	if (p3 && p4) DrawLine((int)c[2].x(), (int)c[2].y(), (int)c[3].x(), (int)c[3].y(), WHITE);
	if (p4 && p1) DrawLine((int)c[3].x(), (int)c[3].y(), (int)c[0].x(), (int)c[0].y(), WHITE);
}

void Window::DrawTriangle(const PPCamera &camera,
	const V3 &point0, const V3 &point1, const V3 &point2,
	const V3 &c0, const V3 &c1, const V3 &c2)
{
	V3 p0, p1, p2;
	camera.ProjectPoint(point0, p0);
	camera.ProjectPoint(point1, p1);
	camera.ProjectPoint(point2, p2);

	auto [bbLeft, bbRight] = std::minmax({p0.x(), p1.x(), p2.x()});
	auto [bbTop, bbBottom] = std::minmax({p0.y(), p1.y(), p2.y()});

	// clip box to window and get point coordinates
	int left = (int) (std::max(bbLeft, 0.0f) - 0.5f);
	int right = (int) (std::min(bbRight, (float) w) + 0.5f);
	int top = (int) (std::max(bbTop, 0.0f) - 0.5f);
	int bottom = (int) (std::min(bbBottom, (float) h) + 0.5f);

	float dy12 = p1.y() - p2.y();
	float dy20 = p2.y() - p0.y();
	float dx21 = p2.x() - p1.x();
	float dx02 = p0.x() - p2.x();
	float div = dy12 * dx02 + dx21 * (p0.y() - p2.y());

	float B0y = (top - p2.y()) * dx21 - dy12 * p2.x();
	float B1y = (top - p2.y()) * dx02 - dy20 * p2.x();

	B0y /= div; B1y /= div; dx21 /= div; dx02 /= div; dy12 /= div; dy20 /= div;

	float B0, B1, B2;

	for (int currPixY = top; currPixY <= bottom; currPixY++, B0y += dx21, B1y += dx02) {
		for (int currPixX = left; currPixX <= right; currPixX++) {
			B0 = dy12 * currPixX + B0y;
			B1 = dy20 * currPixX + B1y;
			B2 = 1.0f - B0 - B1;

			if (B0 >= 0 && B1 >= 0 && B2 >= 0) {
				V3 color = c0 * B0 + c1 * B1 + c2 * B2;
				float z = p0.z() * B0 + p1.z() * B1 + p2.z() * B2;

				SetPixel(currPixX, currPixY, z, color);
			}
		}
	}

}

/*void Window::DrawTriangle(const PPCamera &camera,
	const V3 &point0, const V3 &point1, const V3 &point2,
	const V3 &c0, const V3 &c1, const V3 &c2)
{
	V3 p0, p1, p2;
	camera.ProjectPoint(point0, p0);
	camera.ProjectPoint(point1, p1);
	camera.ProjectPoint(point2, p2);

	// line equation values
	V3 a(
		p1.y() - p0.y(),  // edge 0,1
		p2.y() - p1.y(),  // edge 1,2
		p0.y() - p2.y()   // edge 2,0
	);
	V3 b(
		-p1.x() + p0.x(), // edge 0,1
		-p2.x() + p1.x(), // edge 1,2
		-p0.x() + p2.x()  // edge 2,0
	);
	V3 c(
		-p0.x()*p1.y() + p0.y()*p1.x(), // edge 0,1
		-p1.x()*p2.y() + p1.y()*p2.x(), // edge 1,2
		-p2.x()*p0.y() + p2.y()*p0.x()  // edge 2,0
	);

	// sidedness calculations for each edge

	// edge 0,1
	if (a[0] * p2.x() + b[0] * p2.y() + c[0] < 0) {
		a[0] = -a[0];
		b[0] = -b[0];
		c[0] = -c[0];
	}

	// edge 1,2
	if (a[1] * p0.x() + b[1] * p0.y() + c[1] < 0) {
		a[1] = -a[1];
		b[1] = -b[1];
		c[1] = -c[1];
	}

	// edge 2,0
	if (a[2] * p1.x() + b[2] * p1.y() + c[2] < 0) {
		a[2] = -a[2];
		b[2] = -b[2];
		c[2] = -c[2];
	}

	auto lrBounds = std::minmax({p0.x(), p1.x(), p2.x()});
	auto tbBounds = std::minmax({p0.y(), p1.y(), p2.y()});

	// bounding box calculations
	float boundingBox[4] = {
		// left & right
		lrBounds.first, lrBounds.second,
		// top & bottom
		tbBounds.first, tbBounds.second
	};

	// clip box to window
	if (boundingBox[0] < 0.f) boundingBox[0] = 0.f;
	if (boundingBox[1] >= w) boundingBox[1] = (float) w;
	if (boundingBox[2] < 0.f) boundingBox[2] = 0.f;
	if (boundingBox[3] >= h) boundingBox[3] = (float) h;

	// TODO: this and stuff above can be compressed into a single line for each of l, r, t, b
	int left = (int) (boundingBox[0] + 0.5f);
	int right = (int) (boundingBox[1] - 0.5f);
	int top = (int) (boundingBox[2] + 0.5f);
	int bottom = (int) (boundingBox[3] - 0.5f);

	// edge expression values for line starts and within line
	V3 currEELS, currEE;

	(void) c0, (void) c1, (void) c2;

	for (int i = 0; i < 3; i++) {
		currEELS[i] = a[i] * (left+0.5f) + b[i] * (top+0.5f) + c[i];
	}

	for (int currPixY = top; currPixY <= bottom; currPixY++, currEELS += b) {
		currEE = currEELS;

		for (int currPixX = left; currPixX <= right; currPixX++, currEE += a) {
			if (currEE[0] >= 0 && currEE[1] >= 0 && currEE[2] >= 0) {
				// TODO: find the right z value and color
				// static_assert(0, "TODO: find the right z value and color");
				SetPixel(currPixX, currPixY, 1, c0);
			}
		}
	}

}*/
