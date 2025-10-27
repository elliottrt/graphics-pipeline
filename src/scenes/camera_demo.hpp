#ifndef SCENE_CAMERA_DEMO_HPP
#define SCENE_CAMERA_DEMO_HPP

#include "mesh.hpp"
#include "window.hpp"
#include "scene.hpp"
#include "ppcamera.hpp"

#include <memory>
#include <vector>

struct CameraDemoScene: public Scene {

	std::shared_ptr<Window> wind;

	PPCamera camera;
	Mesh meshes[3];

	PPCamera drawnCamera;

	float pathProgress;
	bool pathPlaying;
	std::vector<std::string> pathFiles;
	size_t pathFrame;

	CameraDemoScene(WindowGroup &group);

	void Update(void) override;
	void Render(void) override;

	void SetCameraOnPath(void);

};

#endif
