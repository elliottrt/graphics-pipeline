#include "tetris.hpp"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_timer.h"
#include "color.hpp"

#include <climits>
#include <cstdlib>
#include <cstring>

TetrisScene::TetrisScene(Window &wind) {
	(void) wind;

	srand(SDL_GetTicksNS() % UINT_MAX);

	framesPerPieceUpdate = 10;

	ResetBoard();

	wallColor = ColorFromRGB(135, 135, 135);

	pieceColors[0] = ColorFromRGB(253, 63, 89);
	pieceColors[1] = ColorFromRGB(255, 200, 46);
	pieceColors[2] = ColorFromRGB(221, 10, 178);
	pieceColors[3] = ColorFromRGB(83, 218, 63);
	pieceColors[4] = ColorFromRGB(1, 237, 250);
	pieceColors[5] = ColorFromRGB(254, 251, 52);
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
	} };
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
	// t piece
	pieceData[2] = TetrisPiece { .rotations = {
		{{0, 1, 0, 0},
		 {1, 1, 1, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},
		{{0, 1, 0, 0},
		 {0, 1, 1, 0},
		 {0, 1, 0, 0},
		 {0, 0, 0, 0}},
		{{0, 0, 0, 0},
		 {1, 1, 1, 0},
		 {0, 1, 0, 0},
		 {0, 0, 0, 0}},
		{{0, 1, 0, 0},
		 {1, 1, 0, 0},
		 {0, 1, 0, 0},
		 {0, 0, 0, 0}},
	} };
	// S piece
	pieceData[3] = TetrisPiece { .rotations = {
		{{0, 1, 1, 0},
		 {1, 1, 0, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},
		{{0, 1, 0, 0},
		 {0, 1, 1, 0},
		 {0, 0, 1, 0},
		 {0, 0, 0, 0}},
		{{0, 0, 0, 0},
		 {0, 1, 1, 0},
		 {1, 1, 0, 0},
		 {0, 0, 0, 0}},
		{{1, 0, 0, 0},
		 {1, 1, 0, 0},
		 {0, 1, 0, 0},
		 {0, 0, 0, 0}},
	} };
	// Z piece
	pieceData[4] = TetrisPiece { .rotations = {
		{{1, 1, 0, 0},
		 {0, 1, 1, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},
		{{0, 0, 1, 0},
		 {0, 1, 1, 0},
		 {0, 1, 0, 0},
		 {0, 0, 0, 0}},
		{{0, 0, 0, 0},
		 {1, 1, 0, 0},
		 {0, 1, 1, 0},
		 {0, 0, 0, 0}},
		{{0, 1, 0, 0},
		 {1, 1, 0, 0},
		 {1, 0, 0, 0},
		 {0, 0, 0, 0}},
	} };
	// backwards L
	pieceData[5] = TetrisPiece { .rotations = {
		{{1, 0, 0, 0},
		 {1, 1, 1, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},
		{{0, 1, 1, 0},
		 {0, 1, 0, 0},
		 {0, 1, 0, 0},
		 {0, 0, 0, 0}},
		{{0, 0, 0, 0},
		 {1, 1, 1, 0},
		 {0, 0, 1, 0},
		 {0, 0, 0, 0}},
		{{0, 1, 0, 0},
		 {0, 1, 0, 0},
		 {1, 1, 0, 0},
		 {0, 0, 0, 0}},
	} };
	// forwards L
	pieceData[6] = TetrisPiece { .rotations = {
		{{0, 0, 1, 0},
		 {1, 1, 1, 0},
		 {0, 0, 0, 0},
		 {0, 0, 0, 0}},
		{{0, 1, 0, 0},
		 {0, 1, 0, 0},
		 {0, 1, 1, 0},
		 {0, 0, 0, 0}},
		{{0, 0, 0, 0},
		 {1, 1, 1, 0},
		 {1, 0, 0, 0},
		 {0, 0, 0, 0}},
		{{1, 1, 0, 0},
		 {0, 1, 0, 0},
		 {0, 1, 0, 0},
		 {0, 0, 0, 0}},
	} };
}

void TetrisScene::Update(Window &wind) {
	if (isDead) {
		deathDelay -= wind.deltaTime;

		if (deathDelay <= 0.0) {
			ResetBoard();
		}
		return;
	}

	// TODO: try to delay user input, otherwise things move too fast and pieces spin many times
	UpdateUserInput(wind);

	// delay execution of piece movement so it isn't too fast.
	// the user can also intentionally drop a piece and update the game
	if (pieceUpdateFrameCounter == framesPerPieceUpdate || wind.KeyPressed(SDL_SCANCODE_DOWN)) {
		pieceUpdateFrameCounter = 0;
	} else {
		pieceUpdateFrameCounter++;
		return;
	}

	if (CanPieceFit(currentPiece, pieceX, pieceY + 1, currentRotation)) {
		pieceY += 1;
	} else {
		InsertPiece();
		ClearFullLines();
		PickRandomPiece();

		// end of game check
		if (!CanPieceFit(currentPiece, pieceX, pieceY, currentRotation)) {
			isDead = true;
			return;
		}
	}
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

	if (isDead) {
		wind.DrawString(0, 0, 6, "YOU DIED\nRESTARTING...", ColorFromRGB(255, 255, 255));
	}
}

void TetrisScene::UpdateUserInput(Window &wind) {
	if (wind.KeyPressed(SDL_SCANCODE_Z)) {
		// try rotating counter clockwise
		int newRotation = RotateBackwards();

		if (CanPieceFit(currentPiece, pieceX, pieceY, newRotation)) {
			currentRotation = newRotation;
		}
	}

	if (wind.KeyPressed(SDL_SCANCODE_X)) {
		// try rotating clockwise
		int newRotation = RotateForwards();

		if (CanPieceFit(currentPiece, pieceX, pieceY, newRotation)) {
			currentRotation = newRotation;
		}
	}

	// piece movement left
	if (wind.KeyPressed(SDL_SCANCODE_LEFT)) {
		if (CanPieceFit(currentPiece, pieceX - 1, pieceY, currentRotation)) {
			pieceX -= 1;
		}
	}

	// piece movement right
	if (wind.KeyPressed(SDL_SCANCODE_RIGHT)) {
		if (CanPieceFit(currentPiece, pieceX + 1, pieceY, currentRotation)) {
			pieceX += 1;
		}
	}
}

void TetrisScene::PickRandomPiece() {
	currentPiece = rand() % PIECE_COUNT;
	currentRotation = rand() % TetrisPiece::ROTATION_COUNT;
	pieceX = WIDTH / 2 - TetrisPiece::BLOCK_SIZE / 2;
	pieceY = 0;
}

void TetrisScene::ResetBoard() {
	// clear board
	for (unsigned row = 0; row < HEIGHT; row++) {
		ClearLine(row);
	}

	PickRandomPiece();

	isDead = false;
	deathDelay = 5.0; // 5 seconds
	pieceUpdateFrameCounter = 0;
}

int TetrisScene::RotateForwards() {
	return (currentRotation + 1) % TetrisPiece::ROTATION_COUNT;
}

int TetrisScene::RotateBackwards() {
	return (currentRotation > 0 ? currentRotation : TetrisPiece::ROTATION_COUNT) - 1;
}

bool TetrisScene::CanPieceFit(int piece, int pieceX, int pieceY, int rotation) {
	const TetrisPiece::RotationData &data = pieceData[piece].rotations[rotation];

	for (int y = 0; y < TetrisPiece::BLOCK_SIZE; y++)  {
		for (int x = 0; x < TetrisPiece::BLOCK_SIZE; x++) {
			bool blockPresent = data[y][x];

			if (blockPresent) {
				// check bottom of board collision
				if (y + pieceY >= (int)HEIGHT) return false;
				// left side
				if (x + pieceX < 0) return false;
				// right side
				if (x + pieceX >= (int)WIDTH) return false;
				
				// check board itself to see if there's already a tile there
				// but not if the piece is above the board
				if (y + pieceY >= 0) {
					if (board[(y + pieceY) * WIDTH + x + pieceX] != 0) return false;
				}
			}
		}
	}

	return true;
}

void TetrisScene::InsertPiece(void) {
	const TetrisPiece::RotationData &data = pieceData[currentPiece].rotations[currentRotation];

	for (int y = 0; y < TetrisPiece::BLOCK_SIZE; y++)  {
		for (int x = 0; x < TetrisPiece::BLOCK_SIZE; x++) {
			bool blockPresent = data[y][x];

			if (blockPresent && y + pieceY >= 0) {
				board[(y + pieceY) * WIDTH + x + pieceX] = currentPiece + 1;
			}
		}
	}
}

void TetrisScene::ClearLine(unsigned line) {
	for (unsigned column = 0; column < WIDTH; column++)
		board[line * WIDTH + column] = 0;
}

bool TetrisScene::IsLineFull(unsigned line) {
	for (unsigned column = 0; column < WIDTH; column++) {
		if (board[line * WIDTH + column] == 0) {
			return false;
		}
	}

	return true;
}

void TetrisScene::ClearFullLines() {

	// special case for the top line, fill it with empty
	if (IsLineFull(0)) {
		ClearLine(0);
	}

	for (unsigned line = 1; line < HEIGHT; line++) {
		if (IsLineFull(line)) {
			int *destLine = &board[1 * WIDTH];
			int *sourceLine = &board[0];
			size_t linesToMove = line;

			// move all lines above down, overwriting this one
			memmove(destLine, sourceLine, linesToMove * WIDTH * sizeof(*board));

			// because all lines are moved down, the top 2 rows will be
			// duplicated. this clears the top line, making it look normal
			ClearLine(0);
		}
	}
}
