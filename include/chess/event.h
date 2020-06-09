#pragma once

class Game;
enum Square;
enum class PieceTypeId;
enum class Colour;

class Event
{
public:
	virtual ~Event() {};

	// Check if event is valid and can be
	// safely applied to game state
	virtual bool isValid(Game const& game) const = 0;

	// Apply event to game state
	virtual void operator()(Game& game) const = 0;
};

class Move : public Event
{
public:
	Move(Square origin, Square dest);

	Square getOrigin() const;
	Square getDestination() const;

	bool isValid(Game const& game) const override;
	void operator()(Game& game) const override;
private:
	Square origin, dest;
};

class Promotion : public Event
{
	bool isValid(Game const& game) const override;
	void operator()(Game& game) const override;
};

class Castling : Event
{
	bool isValid(Game const& game) const override;
	void operator()(Game& game) const override;
};