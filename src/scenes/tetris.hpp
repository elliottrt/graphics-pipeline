#ifndef TETRIS_HPP
#define TETRIS_HPP

#include "window.hpp"
#include "scene.hpp"

#include <cstdint>

struct TetrisPiece {
	constexpr static int ROTATION_COUNT = 4;
	constexpr static int BLOCK_SIZE = 4;

	typedef bool RotationData[BLOCK_SIZE][BLOCK_SIZE];

	// actual representation of the data
	RotationData rotations[ROTATION_COUNT];
};

struct TetrisScene: public Scene {

	constexpr static unsigned PIECE_COUNT = 7;

	// size of game board
	constexpr static unsigned WIDTH = 10;
	constexpr static unsigned HEIGHT = 20;

	// board representation:
	// 0 => empty
	// x => has color pieceColors[x-1]
	int board[WIDTH * HEIGHT];

	uint32_t wallColor;
	uint32_t pieceColors[PIECE_COUNT];
	
	int currentPiece; // index into piece array
	int currentRotation; // index into rotation array
	int pieceX, pieceY; // board index of top left of piece
	TetrisPiece pieceData[PIECE_COUNT];

	// delay for end of game, to show the piece that killed you
	double deathDelay;
	bool isDead;

	// we want to let the player make inputs faster than
	// the pieces go down, so the pieces go down on specific frames
	int framesPerPieceUpdate;
	int pieceUpdateFrameCounter;

	TetrisScene(WindowGroup &group, Window &wind);

	void Update(Window &wind) override;
	void Render(Window &wind) override;

	void UpdateUserInput(Window &wind);

	// select a new piece
	void PickRandomPiece();

	// clears board and resets game state
	void ResetBoard();

	// returns the next rotation
	int RotateForwards();

	// returns the previous notation
	int RotateBackwards();

	// returns whether a piece can fit in the board
	// with the given top-left position and rotation
	bool CanPieceFit(int piece, int x, int y, int rotation);

	// puts the current piece into the board at its position
	void InsertPiece(void);

	// fill given line with empty blocks
	void ClearLine(unsigned line);

	// returns whether the given line is filled with blocks
	bool IsLineFull(unsigned line);

	// clears rows that are completely full
	void ClearFullLines();

};

#endif
