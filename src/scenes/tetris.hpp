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

	TetrisScene(Window &wind);

	void Update(Window &wind) override;
	void Render(Window &wind) override;

	// select a new piece
	void PickRandomPiece();

	// clears board and resets game state
	void ResetBoard();

};

#endif
