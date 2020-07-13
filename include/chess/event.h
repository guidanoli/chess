#pragma once

#include "types.h" // Square

namespace chesslib
{

	class GameState;

	// An event is the parent class of all the possible events that can occurr
	// in a chess game and change the game state.
	class GameEvent
	{
	public:
		virtual ~GameEvent() {};

		// Check if event is valid and can be safely applied to game state.
		virtual bool isValid(GameState const& gameState) = 0;

		// Apply event to game state, if and only if, the event is valid.
		virtual void apply(GameState& gameState) = 0;
	};

	// A move means the displacement of a piece on the board to a different tile.
	// Each piece type has its own way to move, and its own restrictions.
	class Move : public GameEvent
	{
	public:
		// Create a move from an origin to a destination (or dest, for short).
		Move(Square origin, Square dest);

		// Get origin and destination squares.
		Square getOrigin() const;
		Square getDestination() const;

		// Check if move is valid.
		bool isValid(GameState const& gameState) override;

		// Check if move is a valid check, that is, if it could, theoretically,
		// capture an enemy king.
		bool isValidCheck(GameState const& gameState);

		// Apply move to game state.
		void apply(GameState& gameState) override;
	private:
		Square origin, dest;
	};

	// A castling move consists of moving a king two squares towards one of its
	// rooks and placing this rook at the other side of the king.
	// Both pieces must have not been moved until this point and no pieces
	// can be in between the two.
	class Castling : public GameEvent
	{
	public:
		// Create a castling event that involves a rook and the king of same colour
		Castling(Square rook);

		// Get square where rook is located
		Square getRookSquare() const;

		// Check if castling is valid
		bool isValid(GameState const& gameState) override;

		// Apply castling to game state
		void apply(GameState& gameState) override;
	private:
		Square rook;
	};

}