#include "event.h"

#include "types.h"

Move::Move(Square origin, Square dest) :
	origin(origin), dest(dest)
{}

bool Move::isValid() const
{
	return SquareCheck(origin) &&
		   SquareCheck(dest) &&
		   origin != dest;
}

bool Move::operator()(Board& board) const
{
	// 1. How am I going to know if the last move was a pawn
	// walking two ranks and which one was it? (En passant)
	// AW: There has to be information in the Board class
	// indicating such informations

	// 2. The king cannot be captured! Only placed in check!

	// 3. A capture is determined by the final destination
	// having an enemy piece (except for En Passant)

	// 4. The move of each piece type must be validated

	// 5. There must be a piece at the origin and this piece
	// must be of the same colour as the current players'
	// colour. How will this be checked?
	// AW: There has to be a 'turn' information in the board
	
	// 6. It should be checked for pieces that move widely
	// across the board (bishops, queens and rooks), if there
	// are any pieces on the way! How?
	// These pieces have a direction that is multiple of
	// any of the cardinal directions (N, S, W, E, NE, NW, SE, SW).
	// Check first if the direction IS in fact a multiple of
	// any of those directions (which is restricted for given pieces)
	// and add one by one, checking if there are any pieces.

	// 7. A pawn moves strangely. If never moved (rank 2 or 7),
	// it can either move one or two houses towards the enemy side
	// (which depends on the colour). Once moved, it can only move
	// one rank at a time. A pawn can also move diagonally (also in the
	// directed imposed by the colour) if there is a piece there.

	return false;
}