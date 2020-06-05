#pragma once

#include "board.h"

enum Square;

class BoardEvent
{
public:
	virtual ~BoardEvent() = 0;

	// Check if event is valid
	virtual bool isValid() const
	{
		return true;
	}

	// Apply event to board, returning
	// whether it could be applied or not
	virtual bool operator()(Board& board) const
	{
		return true;
	}
};

class Move : BoardEvent
{
public:
	Move(Square origin, Square dest);

	bool isValid() const override;
	bool operator()(Board& board) const override;
private:
	Square origin, dest;
};

class Promotion : BoardEvent
{
	bool isValid() const override;
	bool operator()(Board& board) const override;
};

class Castling : BoardEvent
{
	bool isValid() const override;
	bool operator()(Board& board) const override;
};