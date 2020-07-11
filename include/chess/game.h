#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <memory>

#include "types.h"
#include "board.h"

class Event;

// While playing, the player can make some mistakes
// The way these mistakes are systematically reported is through calls of
// GameListener::catchError passing the corresponding GameError 'id'.

enum class GameError
{
	// Player tries to promote pawn to an illegal piece type
	ILLEGAL_PROMOTION,
};

// Sometimes the game can't figure out everything by itself
// It needs more input from the user for the game must continue to flow
// The way to receive this feedback from the user is by implementing a class
// that inherits from the GameListener class and passing it as an argument
// of the Game class.

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

//   This class represents the game state of a chess match
// Having implemented a listener that allows to communicate with the user,
// the game state is then only changed by successive events fed to the
// Game::update method, alongside calls from the listener.
//
//   The current game state can be visualized by the board class that can
// be obtained by one of the Game::getBoard method (const or not).
//
//   If the game was created in the debug mode, the user has more (pretty
// much always technically illegal) control over the game state. It is aimed
// for debugging or internal purposed only, and may be only set to true if
// truly intended. In most cases, leaving it as false is recommended.

class Game
{
public:
	// Initialize a game
	Game(std::shared_ptr<GameListener> listener,
		 bool debug = false);

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
	Colour getTurn() const;

	// Get en passant pawn
	EnPassantPawn getEnPassantPawn() const;

	// Get game phase
	Phase getPhase() const;

	// Print board
	void pretty() const;
public:
	// Serialize/Deserialize game state
	friend std::ostream& operator<<(std::ostream& fs, Game const& g);
	friend std::istream& operator>>(std::istream& fs, Game& g);

	// Check whether square was altered
	bool wasSquareAltered(Square sq) const;
public:
	// Check if game is in debug mode
	bool isDebug() const;

	// Go to next turn (debug)
	void nextTurn();

	// Set en passant pawn (debug)
	void setEnPassantPawn(EnPassantPawn pawn);

	// Set altered square (debug)
	void setSquareAltered(Square sq, bool altered);
private:
	void privateSetEnPassantPawn(EnPassantPawn pawn);
	void privateNextTurn();
	Square getKingSquare(Colour c) const;
	bool wouldEventCauseCheck(Event* e) const;
	bool canUpdate(Event* e) const;
	void lookForCheckmate();
	void lookForPromotion();
	bool inCheck(Colour c) const;
	void privateSetSquareAltered(Square sq, bool altered);
	bool simulate(std::function<bool(Game&)> cb) const;
private:
	bool m_debug;
	Board m_board;
	Colour m_turn;
	EnPassantPawn m_enpassant_pawn;
	std::shared_ptr<GameListener> m_listener;
	std::map<Square, bool> m_altered_squares;
	Phase m_phase;

	friend class Move;
	friend class Pawn;
};