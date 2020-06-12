#pragma once

#include <array>
#include <optional>

#include "types.h"

class Board
{
public:
	// Create a board with all the pieces in place
	Board();
	// Copy board
	Board(Board const& board);
	// Get piece in a given square
	Piece& operator[](Square sq);
	Piece const& operator[](Square sq) const;
	// Find piece in board
	std::optional<Square> find(PieceTypeId piece_type_id, Colour colour) const;
private:
	std::array<std::array<Piece, 8>, 8> m_matrix;
};