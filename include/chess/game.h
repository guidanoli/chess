#pragma once

#include <fstream>
#include <vector>
#include <functional>

#include "types.h"
#include "board.h"

class Event;

class Game
{
public:
	// Initialize a game
	Game();
	// Copy game state
	Game(Game const& game);
	// Update game state with event
	bool update(Event* e);
	// Get board
	Board& getBoard();
	Board const& getBoard() const;
	// Get turn
	void nextTurn();
	Colour getTurn() const;
	// Get/Set en passant pawn
	EnPassantPawn getEnPassantPawn() const;
	void setEnPassantPawn(EnPassantPawn pawn);
	// Check if move can be made
	bool canMove(Square origin, Square dest) const;
	// Get game phase
	Phase getPhase() const;
	// For debug purposes
	void pretty() const;
	// Serialize/Deserialize
	friend std::ofstream& operator<<(std::ofstream& fs, Game const& g);
	friend std::ifstream& operator>>(std::ifstream& fs, Game& g);
private:
	Square getKingSquare(Colour c) const;
	bool wouldEventCauseCheck(Event* e) const;
	bool canUpdate(Event* e) const;
	bool canUpdateCheck(Move* m) const;
	void lookForCheckmate();
	bool inCheck(Colour c) const;

	// Simulates on a copy
	template<typename Return>
	Return simulate(std::function<Return(Game&)> cb) const {
		return cb(Game(*this));
	}
private:
	Board m_board;
	Colour m_turn;
	EnPassantPawn m_enpassant_pawn;
	Phase m_phase;
};