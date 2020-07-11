#pragma once

#include <memory>

class Game;
enum Square;
enum class Colour;

// An event is the parent class of all the possible events that can occurr
// in a chess game and change the game state.

class Event
{
public:
	virtual ~Event() {};

	// Check if event is valid and can be safely applied to game state.
	virtual bool isValid(Game const& game) = 0;

	// Apply event to game state, if and only if, the event is valid.
	virtual void apply(Game& game) = 0;
};

// A move means the displacement of a piece on the board to a different tile.
// Each piece type has its own way to move, and its own restrictions.

class Move : public Event
{
public:
	// Create a move from an origin to a destination (or dest, for short).
	Move(Square origin, Square dest);

	// Get origin and destination squares.
	Square getOrigin() const;
	Square getDestination() const;

	// Check if move is valid.
	bool isValid(Game const& game) override;

	// Check if move is a valid check, that is, if it could, theoretically,
	// capture an enemy king.
	bool isValidCheck(Game const& game);

	// Apply move to game state.
	void apply(Game& game) override;
private:
	Square origin, dest;
};

class Castling : public Event
{
public:
	// Create a castling event that involves a rook and the king of same colour
	Castling(Square rook);

	// Get square where rook is located
	Square getRookSquare() const;

	// Check if castling is valid
	bool isValid(Game const& game) override;

	// Apply castling to game state
	void apply(Game& game) override;
private:
	Square rook;
};