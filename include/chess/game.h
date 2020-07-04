#pragma once

#include <fstream>
#include <vector>
#include <functional>
#include <memory>

#include "types.h"
#include "board.h"

class Event;

enum class GameError
{
	// Player tries to promote pawn to an illegal piece type
	ILLEGAL_PROMOTION,
};

class GameListener
{
public:
	virtual ~GameListener() {}
	// Asks for a new piece type the pawn at the given square will promote to
	// If the piece type is invalid (NONE, KING or PAWN), the routine will
	// be called repeatedly, and an error message will be set.
	virtual PieceTypeId promotePawn(Game const& game, Square pawn) = 0;
	// Triggers an error alert that should be handled by the listener
	virtual void catchError(Game const& game, GameError err) = 0;
};

class Game
{
public:
	// Initialize a game
	Game(std::shared_ptr<GameListener> listener);
	// Copy game state
	Game(Game const& game);
	// Set new listener
	void setListener(std::shared_ptr<GameListener> listener);
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
	void lookForCheckmate();
	void lookForPromotion();
	bool inCheck(Colour c) const;
	bool simulate(std::function<bool(Game&)> cb) const;
private:
	Board m_board;
	Colour m_turn;
	EnPassantPawn m_enpassant_pawn;
	std::shared_ptr<GameListener> m_listener;
	Phase m_phase;
};