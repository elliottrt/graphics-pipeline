#ifndef SCENE_CAMERA_DEMO_HPP
#define SCENE_CAMERA_DEMO_HPP

#include "mesh.hpp"
#include "window.hpp"
#include "scene.hpp"
#include "ppcamera.hpp"
#include <vector>

struct CameraDemoScene: public Scene {

	PPCamera camera;
	Mesh teapot;

	PPCamera drawnCamera;

	float pathProgress;
	bool pathPlaying;
	std::vector<std::string> pathFiles;

	CameraDemoScene(Window &wind);

	void Update(Window &wind) override;
	void Render(Window &wind) override;

	void SetCameraOnPath(void);

};

#endif
