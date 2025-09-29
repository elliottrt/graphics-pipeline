#include "pong.hpp"
#include "color.hpp"
#include "window.hpp"

#include <algorithm>

PongGame::PongGame(Window &wind) {
	paddleHeight = 100;
	paddleWidth = 20;
	player1PaddleY = wind.h / 2.0 - paddleHeight / 2.0;
	player2PaddleY = player1PaddleY;
	paddlePadding = 20;
	paddleSpeed = 100;

	ballX = wind.w / 2.0;
	ballY = wind.h / 2.0;
	ballVelX = 100;
	ballVelY = ballVelX;
	ballRadius = 30;
}

void PongGame::Update(Window &wind) {
	UpdateBall(wind);
	UpdatePaddles(wind);
}

void PongGame::Render(Window &wind) {
	constexpr uint32_t BLACK = ColorFromRGB(0, 0, 0);
	constexpr uint32_t WHITE = ColorFromRGB(255, 255, 255);

	wind.fb.Clear(BLACK);
	
	wind.fb.DrawCircle((int) ballX, (int) ballY, ballRadius, WHITE);

	// left paddle, player 1
	wind.fb.DrawRect(paddlePadding, (int) player1PaddleY, paddleWidth, paddleHeight, WHITE);

	// right paddle, player 2
	wind.fb.DrawRect(wind.w - paddlePadding - paddleWidth, (int) player2PaddleY, paddleWidth, paddleHeight, WHITE);
}

void PongGame::UpdateBall(Window &wind) {
	ballX += ballVelX * wind.deltaTime;
	ballY += ballVelY * wind.deltaTime;

	// top and bottom edge collisions
	if (ballY - ballRadius <= 0 || ballY + ballRadius >= wind.h)
		ballVelY *= -1;

	// left edge collision, player 1's loss
	if (ballX - ballRadius <= 0) {
		// reset ball
		ballX = wind.w / 2.0;
		ballY = wind.h / 2.0;
		printf("player 1 lost!\n");
	}

	// right edge collision, player 2's loss
	if (ballX + ballRadius >= wind.w) {
		// reset ball
		ballX = wind.w / 2.0;
		ballY = wind.h / 2.0;
		printf("player 2 lost!\n");
	}

	// left paddle, player 1 collision, not very exact
	if (ballX - ballRadius <= paddlePadding + paddleWidth) {
		if (ballY > player1PaddleY && ballY < player1PaddleY + paddleHeight) {
			ballVelX *= -1;
		}
	}

	// right paddle, player 2 collision, not very exact
	if (ballX + ballRadius >= wind.w - (paddlePadding + paddleWidth)) {
		if (ballY > player2PaddleY && ballY < player2PaddleY + paddleHeight) {
			ballVelX *= -1;
		}
	}
}

void PongGame::UpdatePaddles(Window &wind) {
	// left player, using W and S
	if (wind.KeyPressed(SDL_SCANCODE_W))
		player1PaddleY -= paddleSpeed * wind.deltaTime;
	if (wind.KeyPressed(SDL_SCANCODE_S))
		player1PaddleY += paddleSpeed * wind.deltaTime;

	// right player, using up and down
	if (wind.KeyPressed(SDL_SCANCODE_UP))
		player2PaddleY -= paddleSpeed * wind.deltaTime;
	if (wind.KeyPressed(SDL_SCANCODE_DOWN))
		player2PaddleY += paddleSpeed * wind.deltaTime;

	// make sure the paddles stay inside the game
	player1PaddleY = std::clamp(player1PaddleY, 0.0, (double) wind.h - paddleHeight);
	player2PaddleY = std::clamp(player2PaddleY, 0.0, (double) wind.h - paddleHeight);
}
