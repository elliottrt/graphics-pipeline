#include "tetris.hpp"
#include "SDL3/SDL_timer.h"
#include "color.hpp"

#include <climits>
#include <random>

TetrisScene::TetrisScene(Window &wind): Scene(wind) {
	srand(SDL_GetTicksNS() % UINT_MAX);

	ResetBoard();

	wallColor = ColorFromRGB(135, 135, 135);

	pieceColors[0] = ColorFromRGB(253, 63, 89);
	pieceColors[1] = ColorFromRGB(255, 200, 46);
	pieceColors[2] = ColorFromRGB(254, 251, 52);
	pieceColors[3] = ColorFromRGB(83, 218, 63);
	pieceColors[4] = ColorFromRGB(1, 237, 250);
	pieceColors[5] = ColorFromRGB(221, 10, 178);
	pieceColors[6] = ColorFromRGB(46, 46, 132);

	// square piece
	pieceData[0] = TetrisPiece { .rotations = {
		{{0, 0, 0, 0},
		 {0, 1, 1, 0},
		 {0, 1, 1, 0},
		 {0, 0, 0, 0}},
		{{0, 0, 0, 0},
		 {0, 1, 1, 0},
		 {0, 1, 1, 0},
		 {0, 0, 0, 0}},
		{{0, 0, 0, 0},
		 {0, 1, 1, 0},
		 {0, 1, 1, 0},
		 {0, 0, 0, 0}},
		{{0, 0, 0, 0},
		 {0, 1, 1, 0},
		 {0, 1, 1, 0},
		 {0, 0, 0, 0}},
	}};
	// long piece
	pieceData[1] = TetrisPiece { .rotations = {
		{{0, 0, 0, 0},
		 {1, 1, 1, 1},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},
		{{0, 0, 1, 0},
		 {0, 0, 1, 0},
		 {0, 0, 1, 0},
		 {0, 0, 1, 0}},
		{{0, 0, 0, 0},
		 {0, 0, 0, 0},
		 {1, 1, 1, 1},
		 {0, 0, 0, 0}},
		{{0, 1, 0, 0},
		 {0, 1, 0, 0},
		 {0, 1, 0, 0},
		 {0, 1, 0, 0}},
	} };
	pieceData[2] = TetrisPiece { .rotations = {} };
	pieceData[3] = TetrisPiece { .rotations = {} };
	pieceData[4] = TetrisPiece { .rotations = {} };
	pieceData[5] = TetrisPiece { .rotations = {} };
	pieceData[6] = TetrisPiece { .rotations = {} };
}

void TetrisScene::Update(Window &wind) {
	(void) wind;
}

void TetrisScene::Render(Window &wind) {
	constexpr uint32_t BLACK = ColorFromRGB(0x18, 0x18, 0x18);

	// reset board
	wind.Clear(BLACK);

	unsigned tileSize = wind.h / HEIGHT;
	unsigned uStart = wind.w / 2 - WIDTH * tileSize / 2;
	
	// draw the board
	for (unsigned v = 0; v < HEIGHT; v++) {
		for (unsigned u = 0; u < WIDTH; u++) {
			wind.DrawRect(
				u * tileSize + uStart,
				v * tileSize,
				tileSize,
				tileSize,
				board[v * WIDTH + u] == 0 ? BLACK : pieceColors[board[v * WIDTH + u]-1]
			);
		}
	}

	// draw board edges
	wind.DrawRect(uStart - tileSize, 0, tileSize, wind.h, wallColor);
	wind.DrawRect(uStart + WIDTH * tileSize, 0, tileSize, wind.h, wallColor);

	// draw the moving piece
	const TetrisPiece::RotationData &data = pieceData[currentPiece].rotations[currentRotation];

	for (int v = 0; v < TetrisPiece::BLOCK_SIZE; v++) {
		for (int u = 0; u < TetrisPiece::BLOCK_SIZE; u++) {
			if (data[v][u]) {
				wind.DrawRect(
					(u + pieceX) * tileSize + uStart,
					(v + pieceY) * tileSize,
					tileSize,
					tileSize,
					pieceColors[currentPiece]
				);
			}
		}
	}
}

void TetrisScene::PickRandomPiece() {
	currentPiece = rand() % PIECE_COUNT;
	currentRotation = 0;
	pieceX = WIDTH - TetrisPiece::BLOCK_SIZE / 2;
	pieceY = 0;
}

void TetrisScene::ResetBoard() {
	// clear board
	for (unsigned i = 0; i < WIDTH * HEIGHT; i++) {
		board[i] = 0;
	}

	PickRandomPiece();
}
