#include "event.h"

#include "game.h"
#include "board.h"
#include "types.h"

Move::Move(Square origin, Square dest) :
	origin(origin), dest(dest)
{}

bool Move::isValid(Game const& game)
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

bool Move::isValidCheck(Game const& game)
{
	if (!SquareCheck(origin) || !SquareCheck(dest) || origin == dest)
		return false;

	auto const& moved_piece = game.getBoard()[origin];

	if (moved_piece.getColour() != game.getTurn())
		return false;

	auto const& captured_piece = game.getBoard()[dest];

	if (*captured_piece.getType() != PieceTypeId::KING ||
		captured_piece.getColour() == game.getTurn())
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

void Move::apply(Game& game)
{
	auto& board = game.getBoard();
	auto& origpiece = board[origin];
	auto& destpiece = board[dest];
	
	destpiece = origpiece;
	origpiece.clear();

	destpiece.getType()->afterApplied(game, *this);

	game.privateSetSquareAltered(origin, true);
	game.privateSetSquareAltered(dest, true);
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
	
	if (dir != DIR_NORTHWEST &&
		dir != DIR_SOUTHWEST &&
		dir != DIR_NORTHEAST &&
		dir != DIR_SOUTHEAST)
		return false;

	while (true) {
		
		// Goes one move further in the direction
		orig += dir;

		// Got out of the board
		if (!SquareCheck(orig))
			return false;

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

	if (dir != DIR_NORTH &&
		dir != DIR_SOUTH &&
		dir != DIR_WEST &&
		dir != DIR_EAST)
		return false;

	while (true) {

		// Goes one move further in the direction
		orig += dir;

		// Got out of the board
		if (!SquareCheck(orig))
			return false;

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

		// Goes one move further in the direction
		orig += dir;

		// Got out of the board
		if (!SquareCheck(orig))
			return false;

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

void Pawn::afterApplied(Game& g, Move const& m) const
{
	Direction dir = m.getDestination() - m.getOrigin();

	if (dir == DIR_NORTH * 2 || dir == DIR_SOUTH * 2) {
		Direction half_dir = Direction((int) dir / 2);
		g.privateSetEnPassantPawn(square2EnPassant(m.getOrigin() + half_dir));
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
				g.getBoard()[current_pawn_sq].clear();
			}
		}
	}
}

Castling::Castling(Square rook) : rook(rook) {}

Square Castling::getRookSquare() const { return rook; }

bool Castling::isValid(Game const& game)
{
	// Rook must be in one of the four corners of the board
	if (rook != SQ_A1 && rook != SQ_A8 && rook != SQ_H1 && rook != SQ_H8)
		return false;

	const auto& rook_piece = game.getBoard()[rook];

	// There must be a rook at the given position
	if (*rook_piece.getType() != PieceTypeId::ROOK)
		return false;

	bool white_rook = rook_piece.getColour() == Colour::WHITE;
	Square king = white_rook ? SQ_D1 : SQ_D8;

	const auto& king_piece = game.getBoard()[king];

	// There must be a king at the given position
	if (*king_piece.getType() != PieceTypeId::KING)
		return false;

	// Both pieces must have never been moved
	// This already implies they are of same colour
	if (game.wasSquareAltered(rook) || game.wasSquareAltered(king))
		return false;

	return true;
}

void Castling::apply(Game& game)
{
	const auto& rook_piece = game.getBoard()[rook];
	bool white_rook = rook_piece.getColour() == Colour::WHITE;
	Square king = white_rook ? SQ_D1 : SQ_D8;

	Direction king_dir = (king < rook) ? DIR_EAST : DIR_WEST;

	Square king_dest = king + 2 * king_dir;
	Square rook_dest = king_dest - king_dir;

	Move(king, king_dest).apply(game);
	Move(rook, rook_dest).apply(game);
}