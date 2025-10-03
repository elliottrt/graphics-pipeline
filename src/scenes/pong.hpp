#ifndef GAMES_PONG_HPP
#define GAMES_PONG_HPP

#include "window.hpp"
#include "scene.hpp"

// we use WASD for the left player and arrow keys for the right player

struct PongGame : public Scene {
	int paddleHeight, paddleWidth;
	double player1PaddleY;
	double player2PaddleY;
	// distance from the edge of the window
	int paddlePadding;
	double paddleSpeed;

	double ballX, ballY;
	double ballVelX, ballVelY;
	int ballRadius;
	
	PongGame(WindowGroup &group, Window &wind);

	void Update(Window &wind) override;
	void Render(Window &wind) override;

	void UpdateBall(Window &wind);
	void UpdatePaddles(Window &wind);
};

#endif // GAMES_PONG_HPP
