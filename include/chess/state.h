#pragma once

#include <iosfwd>
#include <map>
#include <vector>
#include <functional>
#include <memory>

#include "board.h"

class GameListener;
enum class GameError;
enum class Colour;
enum class EnPassantPawn;
enum class Phase;
enum Square;
class GameEvent;
class GameStateController;
class GameStateControllerConst;
class DebugGameState;
class DebugGameStateController;

//   This class represents the game state of a chess match
// Having implemented a listener that allows to communicate with the user,
// the game state is then only changed by successive events fed to the
// GameState::update method, alongside calls from the listener.
//
//   The current game state can be visualized by the board class that can
// be obtained by one of the GameState::getBoard method (const or not).
//
//   If the game was created in the debug mode, the user has more (pretty
// much always technically illegal) control over the game state. It is aimed
// for debugging or internal purposed only, and may be only set to true if
// truly intended. In most cases, leaving it as false is recommended.
class GameState
{
public:
	// Initialize a game
	GameState(std::shared_ptr<GameListener> listener);

	// Copy game state
	GameState(GameState const& game);

	// Set new listener
	void setListener(std::shared_ptr<GameListener> listener);

	// Update game state with event
	bool update(GameEvent* e);

	// Get board
	Board& getBoard();
	Board const& getBoard() const;

	// Get turn
	Colour getTurn() const;

	// Get en passant pawn
	EnPassantPawn getEnPassantPawn() const;

	// Get game phase
	Phase getPhase() const;

	// Print board in a nice ASCII-style
	void pretty(std::ostream& os);
public:
	// Serialize/Deserialize game state
	friend std::ostream& operator<<(std::ostream& out, GameState const& g);
	friend std::istream& operator>>(std::istream& in, GameState& g);

	// Get constant controller
	GameStateControllerConst getConstController() const;
private:
	void nextTurn();
	bool wasSquareAltered(Square sq) const;
	void raiseError(GameError err) const;
	GameStateController getController();
	DebugGameStateController getDebugController();
	void setEnPassantPawn(EnPassantPawn pawn);
	Square getKingSquare(Colour c) const;
	bool wouldEventCauseCheck(GameEvent* e) const;
	bool canUpdate(GameEvent* e) const;
	void lookForCheckmate();
	void lookForPromotion();
	bool inCheck(Colour c) const;
	void setSquareAltered(Square sq, bool altered);
	bool simulate(std::function<bool(GameState&)> cb) const;
private:
	Board m_board;
	Colour m_turn;
	EnPassantPawn m_enpassant_pawn;
	std::shared_ptr<GameListener> m_listener;
	std::map<Square, bool> m_altered_squares;
	Phase m_phase;

	friend class DebugGameState;
	friend class DebugGameStateController;
	friend class GameStateController;
	friend class GameStateControllerConst;
};
