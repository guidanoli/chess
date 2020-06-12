#pragma once

#include <memory>

class Game;
enum Square;
enum class Colour;
class PieceType;
enum class PieceTypeId;

class Event
{
public:
	virtual ~Event() {};

	// Check if event is valid and can be
	// safely applied to game state
	virtual bool isValid(Game const& game) = 0;

	// Apply event to game state
	virtual void apply(Game& game) = 0;
};

class Move : public Event
{
public:
	Move(Square origin, Square dest);

	Square getOrigin() const;
	Square getDestination() const;

	bool isValid(Game const& game) override;
	bool isValidCheck(Game const& game);
	void apply(Game& game) override;
private:
	Square origin, dest;
};