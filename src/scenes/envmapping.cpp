#include "envmapping.hpp"
#include "cube_map.hpp"
#include "ppcamera.hpp"
#include "scene.hpp"

static const std::array<std::string, CubeMap::N> sides = {
	"geometry/uffizi_front.tiff",
	"geometry/uffizi_left.tiff",
	"geometry/uffizi_back.tiff",
	"geometry/uffizi_right.tiff",
	"geometry/uffizi_top.tiff",
	"geometry/uffizi_bottom.tiff"
};

EnvironmentMappingScene::EnvironmentMappingScene(WindowGroup &group, Window &wind):
	Scene(group), map(sides),
	camera(wind.w, wind.h, 60.0f)
{
	obj.Load("geometry/teapot57K.bin");
	obj.TranslateTo(V3(0, 0, -120));

	// move the map to the middle of the object
	for (auto &c : map.cameras)
		c.C = obj.GetCenter();
}

void EnvironmentMappingScene::Update(Window &wind) {
	static constexpr float speed = 2.5f;

	V3 movement;
	movement.x() = (float)wind.KeyPressed(SDL_SCANCODE_D) - (float)wind.KeyPressed(SDL_SCANCODE_A);
	movement.y() = (float)wind.KeyPressed(SDL_SCANCODE_SPACE) - (float)(wind.KeyPressed(SDL_SCANCODE_LSHIFT) || wind.KeyPressed(SDL_SCANCODE_RSHIFT));
	movement.z() = (float)wind.KeyPressed(SDL_SCANCODE_S) - (float)wind.KeyPressed(SDL_SCANCODE_W);

	// pan
	if (movement.x() != 0.0f) {
		camera.Pose(
			camera.C.RotateAroundAxis(obj.GetCenter(), -camera.b, movement.x() * speed),
			obj.GetCenter(),
			-camera.b
		);
	}

	// tilt
	if (movement.y() != 0.0f) {
		// TODO: restrict this from going too low or too high
		camera.Pose(
			camera.C.RotateAroundAxis(obj.GetCenter(), -camera.a, movement.y() * speed),
			obj.GetCenter(),
			-camera.b
		);
	}

	// roll
	if (movement.z() != 0.0f) {
		camera.Roll(movement.z() * speed);
	}
}

void EnvironmentMappingScene::Render(Window &wind) {
	wind.fb.Clear(0);
	obj.DrawFilledEnvMap(wind.fb, camera, map);

	for (const auto &c : map.cameras) wind.fb.DrawCamera(camera, c);
}
