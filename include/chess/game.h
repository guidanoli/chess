#pragma once

#include "types.h"
#include "board.h"

class Event;

class Game
{
public:
	// Initialize a game
	Game();
	// Update game state with event
	bool update(Event& e);
	// Get board
	Board& getBoard();
	Board const& getBoard() const;
	// Get turn
	Colour getTurn() const;
	// Get/Set en passant pawn
	EnPassantPawn getEnPassantPawn() const;
	void setEnPassantPawn(EnPassantPawn pawn);
private:
	Board m_board;
	Colour m_turn;
	EnPassantPawn m_enpassant_pawn;
};