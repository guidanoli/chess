#pragma once

namespace chesslib
{

	enum class GameError;
	class GameController;
	enum class PieceTypeId;
	enum Square;

	// Sometimes the game can't figure out everything by itself
	// It needs more input from the user for the game must continue to flow
	// The way to receive this feedback from the user is by implementing a class
	// that inherits from the GameListener class and passing it as an argument
	// of the GameState class.
	class GameListener
	{
	public:
		virtual ~GameListener() {}

		// Asks for a new piece type the pawn at the given square will promote to
		// If the piece type is invalid (NONE, KING or PAWN), the routine will
		// be called repeatedly, and an error message will be set.
		virtual PieceTypeId promotePawn(GameController const& gameController, Square pawn) = 0;

		// Triggers an error alert that should be handled by the listener
		virtual void catchError(GameController const& gameController, GameError err) = 0;
	};

}