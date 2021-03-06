#pragma once

#include <iosfwd> // std::ostream
#include <optional> // std::optional

#include "types.h" // Piece, Square, PieceTypeId, Colour

namespace chesslib
{

	// Originally named 'BoardView', this class aims to aid visualizing the
	// current game state by informing which piece is in which tile of the board.
	// Of course there can be also no piece at all, in this case, the piece will
	// be 'NONE'. For more information, see also the 'PieceTypeId' enum class.
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

		// Display board in a pretty ASCII style
		void pretty(std::ostream& os) const;

		// Find piece in board
		std::optional<Square> find(PieceTypeId piece_type_id, Colour colour) const;
	private:
		Piece m_matrix[SQ_CNT];
	};

}