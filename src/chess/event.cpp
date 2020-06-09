#include "event.h"

#include "game.h"
#include "board.h"
#include "types.h"

Move::Move(Square origin, Square dest) :
	origin(origin), dest(dest)
{}

bool Move::isValid(Game const& game) const
{
	if (!SquareCheck(origin) || !SquareCheck(dest) || origin == dest)
		return false;

	auto const& moved_piece = game.getBoard()[origin];

	if (moved_piece.getColour() != game.getTurn())
		return false;

	auto const& captured_piece = game.getBoard()[dest];

	if (*captured_piece.getType() != PieceTypeId::NONE &&
		captured_piece.getColour() == game.getTurn())
		return false;

	if (*captured_piece.getType() == PieceTypeId::KING)
		return false;

	return moved_piece.getType()->canApply(game, *this);
}

Square Move::getOrigin() const
{
	return origin;
}

Square Move::getDestination() const
{
	return dest;
}

void Move::operator()(Game& game) const
{
	auto& board = game.getBoard();
	auto& origpiece = board[origin];
	auto& destpiece = board[dest];
	
	destpiece = origpiece;
	origpiece.setType(std::make_shared<EmptyTile>());

	destpiece.getType()->afterApplied(game, *this);
}

bool Pawn::canApply(Game const& g, Move const& m) const
{
	auto orig = m.getOrigin();
	auto dest = m.getDestination();
	Direction white_dir = dest - orig;
	auto white_orig = orig;
	auto const& origpiece = g.getBoard()[orig];
	auto const& destpiece = g.getBoard()[dest];

	if (origpiece.getColour() == Colour::BLACK) {
		white_orig = ~white_orig;
		white_dir = -white_dir;
	}

	auto enpassant = g.getEnPassantPawn();
	auto enpassant_sq = getEnPassantPawnSquare(enpassant);

	if (*destpiece.getType() == PieceTypeId::NONE && enpassant_sq != dest) {
		return (getSquareRank(white_orig) == RK_2 &&
			    white_dir == DIR_NORTH * 2) ||
			   white_dir == DIR_NORTH;
	} else {
		return white_dir == DIR_NORTHEAST ||
			   white_dir == DIR_NORTHWEST;
	}
}

bool King::canApply(Game const& g, Move const& m) const
{
	auto orig = m.getOrigin();
	auto dest = m.getDestination();

	auto orig_rank = getSquareRank(orig);
	auto orig_file = getSquareFile(orig);

	return (orig_rank != RK_1 && orig + DIR_SOUTH == dest) ||
		   (orig_rank != RK_8 && orig + DIR_NORTH == dest) ||
		   (orig_file != FL_A && orig + DIR_WEST  == dest) ||
		   (orig_file != FL_H && orig + DIR_EAST  == dest);
}

bool Knight::canApply(Game const& g, Move const& m) const
{
	auto dir = m.getDestination() - m.getOrigin();

	return dir == DIR_NORTH * 2 + DIR_EAST ||
		   dir == DIR_NORTH * 2 + DIR_WEST ||
		   dir == DIR_SOUTH * 2 + DIR_EAST ||
		   dir == DIR_SOUTH * 2 + DIR_WEST ||
		   dir == DIR_NORTH + 2 * DIR_EAST ||
		   dir == DIR_NORTH + 2 * DIR_WEST ||
		   dir == DIR_SOUTH + 2 * DIR_EAST ||
		   dir == DIR_SOUTH + 2 * DIR_WEST;
}

bool Bishop::canApply(Game const& g, Move const& m) const
{
	auto orig = m.getOrigin();
	auto dest = m.getDestination();

	auto orig_rank = getSquareRank(orig);
	auto orig_file = getSquareFile(orig);

	auto dest_rank = getSquareRank(dest);
	auto dest_file = getSquareFile(dest);

	Direction dir = DIR_NONE;

	if (dest_file > orig_file)
		dir += DIR_EAST;
	else if (dest_file < orig_file)
		dir += DIR_WEST;

	if (dest_rank > orig_rank)
		dir += DIR_NORTH;
	else if (dest_rank < orig_rank)
		dir += DIR_SOUTH;
	
	while (true) {
		
		// Check if, if followed a diagonal,
		// would the piece fall off the board.
		switch (dir) {
		case DIR_NORTHWEST:
			if (orig_rank == RK_8 ||
				orig_file == FL_A)
				return false;
			break;
		case DIR_NORTHEAST:
			if (orig_rank == RK_8 ||
				orig_file == FL_H)
				return false;
			break;
		case DIR_SOUTHWEST:
			if (orig_rank == RK_1 ||
				orig_file == FL_A)
				return false;
			break;
		case DIR_SOUTHEAST:
			if (orig_rank == RK_1 ||
				orig_file == FL_H)
				return false;
			break;
		default:
			// Invalid direction
			return false;
		}

		// Goes one move further in the direction
		orig += dir;

		// If has reached the destination, it is a diagonal
		if (orig == dest)
			return true;

		// If hasn't reached yet, there must be no piece there!
		if (*g.getBoard()[orig].getType() != PieceTypeId::NONE)
			return false;

		// Update original square's rank and file
		auto orig_rank = getSquareRank(orig);
		auto orig_file = getSquareFile(orig);
	}
}

bool Rook::canApply(Game const& g, Move const& m) const
{
	auto orig = m.getOrigin();
	auto dest = m.getDestination();

	auto orig_rank = getSquareRank(orig);
	auto orig_file = getSquareFile(orig);

	auto dest_rank = getSquareRank(dest);
	auto dest_file = getSquareFile(dest);

	Direction dir = DIR_NONE;

	if (dest_file > orig_file)
		dir += DIR_EAST;
	else if (dest_file < orig_file)
		dir += DIR_WEST;

	if (dest_rank > orig_rank)
		dir += DIR_NORTH;
	else if (dest_rank < orig_rank)
		dir += DIR_SOUTH;

	while (true) {

		// Check if, if followed a line,
		// would the piece fall off the board.
		switch (dir) {
		case DIR_NORTH:
			if (orig_rank == RK_8)
				return false;
			break;
		case DIR_SOUTH:
			if (orig_rank == RK_1)
				return false;
			break;
		case DIR_WEST:
			if (orig_file == FL_A)
				return false;
			break;
		case DIR_EAST:
			if (orig_file == FL_H)
				return false;
			break;
		default:
			// Invalid direction
			return false;
		}

		// Goes one move further in the direction
		orig += dir;

		// If has reached the destination, it is a line
		if (orig == dest)
			return true;

		// If hasn't reached yet, there must be no piece there!
		if (*g.getBoard()[orig].getType() != PieceTypeId::NONE)
			return false;

		// Update original square's rank and file
		auto orig_rank = getSquareRank(orig);
		auto orig_file = getSquareFile(orig);
	}
}

bool Queen::canApply(Game const& g, Move const& m) const
{
	return Rook().canApply(g, m) || Bishop().canApply(g, m);
}

void Pawn::afterApplied(Game& g, Move const& m) const
{
	Direction dir = m.getDestination() - m.getOrigin();

	if (dir == DIR_NORTH * 2 || dir == DIR_SOUTH * 2) {
		Direction half_dir = Direction((int) dir / 2);
		g.setEnPassantPawn(square2EnPassant(m.getOrigin() + half_dir));
	} else {
		EnPassantPawn enpassant = g.getEnPassantPawn();
		if (enpassant != EnPassantPawn::NONE) {
			Square enpassant_sq = getEnPassantPawnSquare(enpassant);
			if (enpassant_sq == m.getDestination()) {
				Square current_pawn_sq = enpassant_sq;
				if (getSquareRank(enpassant_sq) == RK_3)
					current_pawn_sq += DIR_NORTH; // White pawn
				else
					current_pawn_sq += DIR_SOUTH; // Black pawn
				g.getBoard()[current_pawn_sq].setType(std::make_shared<EmptyTile>());
			}
		}
	}
}