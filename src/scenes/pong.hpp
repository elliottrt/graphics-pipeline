#ifndef GAMES_PONG_HPP
#define GAMES_PONG_HPP

#include "window.hpp"
#include "scene.hpp"
#include <memory>

// we use WASD for the left player and arrow keys for the right player

struct PongGame : public Scene {
	std::shared_ptr<Window> wind;

	int paddleHeight, paddleWidth;
	double player1PaddleY;
	double player2PaddleY;
	// distance from the edge of the window
	int paddlePadding;
	double paddleSpeed;

	double ballX, ballY;
	double ballVelX, ballVelY;
	int ballRadius;
	
	PongGame(WindowGroup &group);

	void Update() override;
	void Render() override;

	void UpdateBall();
	void UpdatePaddles();
};

#endif // GAMES_PONG_HPP
