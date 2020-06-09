#pragma once

#include <array>

#include "types.h"

enum Square;

class Board
{
public:
	// Create a board with all the pieces in place
	Board();
	// Get piece in a given square
	Piece& operator[](Square sq);
	Piece const& operator[](Square sq) const;
private:
	std::array<std::array<Piece, 8>, 8> m_matrix;
};