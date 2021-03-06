#pragma once

#include <iosfwd> // std::istream, std::ostream

#include "board.h" // Board
#include "types.h" // Colour, Phase, Square
#include "error.h" // GameError

namespace chesslib
{

	// This class represents a game state, but does not provide
	// any business logic whatsoever
	class GameState
	{
	public:
		// Initialize a game
		GameState();

		// Copy a game state
		GameState(GameState const& other);

		// Skip to next turn
		void nextTurn();

		// Get turn
		Colour getTurn() const;

		// Set game phase
		void setPhase(Phase phase);

		// Get game phase
		Phase getPhase() const;

		// Move piece
		void movePiece(Square origin, Square dest);

		// Set/Get piece at square
		Piece& getPieceAt(Square sq);

		// Get piece at square
		Piece const& getPieceAt(Square sq) const;

		// Clear square
		void clearSquare(Square sq);

		// Set/Get board
		Board& getBoard();

		// Get board (const)
		Board const& getBoard() const;

		// Has en passant square set
		bool hasEnPassant() const;

		// Set en passant pawn
		void setEnPassantPawn(Square pawn);

		// Clear en passant pawn
		void clearEnPassantPawn();

		// Get en passant pawn
		Square getEnPassantPawn() const;

		// Set (in)altered square
		void setSquareAltered(Square sq, bool altered);

		// Check whether square was altered
		bool wasSquareAltered(Square sq) const;

		// Deserialize game state
		// Throws GameError in case of error
		void load(std::istream& in);

		// Serialize game state
		void save(std::ostream& out) const;
	private:
		Board m_board;
		Colour m_turn;
		Phase m_phase;
		bool m_altered_map[SQ_CNT];
		Square m_enpassant_pawn;
	};

	inline bool EnPassantPawnCheck(Square sq)
	{
		auto const rk = getSquareRank(sq);
		return sq == SQ_CNT || rk == RK_3 || rk == RK_6;
	}

}
