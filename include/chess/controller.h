#pragma once

#include <memory> // std::unique_tr, std::shared_ptr
#include <iosfwd> // std::istream, std::ostream
#include <functional> // std::function

#include "error.h" // GameError
#include "types.h" // Colour, Square

namespace chesslib
{

	class GameEvent;
	class GameState;
	class GameListener;

	// This is the class responsible for controlling the chess game
	// state behing some business logic, fed with GameEvents.
	class GameController
	{
	public:
		// Create a game controller to control a unique game state
		// and with a shared listener
		GameController(std::unique_ptr<GameState> gameStatePtr,
		               std::shared_ptr<GameListener> listener);

		// Copy a game controller, which copies the game state and
		// shares the same listener
		GameController(GameController const& other);

		// Get current game state
		GameState const& getState() const;

		// Update game state with a game event
		// Returns true on success
		bool update(std::shared_ptr<GameEvent> event);

		// Load game state from input stream
		// Returns true on success
		bool loadState(std::istream& is);

		// Save game state to output stream
		// Returns true on success
		bool saveState(std::ostream& os) const;
	private:
		// Replace listener with a dummy one
		void clearListener();

		// Check whether player of colour c is in check
		bool inCheck(Colour c) const;

		// Obtain square in which the king of colour c is located on
		Square getKingSquare(Colour c) const;

		// Look for a pawn that should be promoted instantly
		void lookForPromotion();

		// Look for a checkmate that occurred immediately
		void lookForCheckmate();

		// Check whether game state can be updated with event, that is,
		// so that the player tha makes the move doesn't put himself in check
		bool canUpdate(std::shared_ptr<GameEvent> e) const;

		// Check whether after an event would cause a check
		bool wouldEventCauseCheck(std::shared_ptr<GameEvent> e) const;

		// Raise a game error to the listener
		void raiseError(GameError err) const;

		// Simulate a predicate with a clone of this controller
		using simulationCallback = std::function<bool(GameController&)>;
		bool simulate(simulationCallback cb) const;
	private:
		std::unique_ptr<GameState> m_state;
		std::shared_ptr<GameListener> m_listener;
	};

}