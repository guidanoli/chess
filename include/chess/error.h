#pragma once

// While playing, the player can make some mistakes
// The way these mistakes are systematically reported is through calls of
// GameListener::catchError passing the corresponding GameError 'id'.
enum class GameError
{
	// Player tries to promote pawn to an illegal piece type
	ILLEGAL_PROMOTION,

	// Incompatible version
	IO_VERSION,

	// Invalid turn
	IO_TURN,

	// Invalid en passant rank
	IO_EN_PASSANT_RANK,

	// Invalid en passant file
	IO_EN_PASSANT_FILE,

	// Invalid square
	IO_SQUARE,

	// Invalid colour
	IO_COLOUR,

	// Invalid piece type
	IO_PIECE_TYPE,
};