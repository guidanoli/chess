#include "event.h"

#include "state.h"

using namespace chesslib;

Move::Move(Square origin, Square dest) :
	origin(origin), dest(dest)
{}

bool Move::isValid(GameState const& game)
{
	if (!SquareCheck(origin) || !SquareCheck(dest) || origin == dest)
		return false;

	auto const& moved_piece = game.getPieceAt(origin);

	if (moved_piece.getColour() != game.getTurn())
		return false;

	auto const& captured_piece = game.getPieceAt(dest);

	if (!captured_piece.isClear() &&
		captured_piece.getColour() == game.getTurn())
		return false;

	if (captured_piece.getType()->getId() == PieceTypeId::KING)
		return false;

	return moved_piece.getType()->canApply(game, *this);
}

bool Move::isValidCheck(GameState const& game)
{
	if (!SquareCheck(origin) || !SquareCheck(dest) || origin == dest)
		return false;

	auto const& moved_piece = game.getPieceAt(origin);

	if (moved_piece.getColour() != game.getTurn())
		return false;

	auto const& captured_piece = game.getPieceAt(dest);

	if (captured_piece.getType()->getId() != PieceTypeId::KING ||
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

void Move::apply(GameState& game)
{
	auto destpiece = game.getPieceAt(dest);

	game.movePiece(origin, dest);

	destpiece.getType()->afterApplied(game, *this);
}

bool Pawn::canApply(GameState const& g, Move const& m) const
{
	auto orig = m.getOrigin();
	auto dest = m.getDestination();
	Direction white_dir = dest - orig;
	auto white_orig = orig;
	auto const& origpiece = g.getPieceAt(orig);
	auto const& destpiece = g.getPieceAt(dest);

	if (origpiece.getColour() == Colour::BLACK) {
		white_orig = ~white_orig;
		white_dir = -white_dir;
	}

	auto enpassant = g.getEnPassantPawn();
	auto enpassant_sq = static_cast<Square>(enpassant);

	if (destpiece.isClear() && enpassant_sq != dest) {
		return (getSquareRank(white_orig) == RK_2 &&
			    white_dir == DIR_NORTH * 2) ||
			   white_dir == DIR_NORTH;
	} else {
		return white_dir == DIR_NORTHEAST ||
			   white_dir == DIR_NORTHWEST;
	}
}

bool King::canApply(GameState const& g, Move const& m) const
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

bool Knight::canApply(GameState const& g, Move const& m) const
{
	auto const orig = m.getOrigin();
	auto const dest = m.getDestination();

	auto const orig_rank = getSquareRank(orig);
	auto const orig_file = getSquareFile(orig);

	auto const dest_rank = getSquareRank(dest);
	auto const dest_file = getSquareFile(dest);

	auto const rank_diff = static_cast<int>(orig_rank) - static_cast<int>(dest_rank);
	auto const file_diff = static_cast<int>(orig_file) - static_cast<int>(dest_file);

	if (rank_diff == 0 || file_diff == 0)
		return false;

	return rank_diff + file_diff == 3;
}

bool Bishop::canApply(GameState const& g, Move const& m) const
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
		if (!g.getPieceAt(orig).isClear())
			return false;

		// Update original square's rank and file
		auto orig_rank = getSquareRank(orig);
		auto orig_file = getSquareFile(orig);
	}
}

bool Rook::canApply(GameState const& g, Move const& m) const
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
		if (!g.getPieceAt(orig).isClear())
			return false;

		// Update original square's rank and file
		auto orig_rank = getSquareRank(orig);
		auto orig_file = getSquareFile(orig);
	}
}

bool Queen::canApply(GameState const& g, Move const& m) const
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
		if (!g.getPieceAt(orig).isClear())
			return false;

		// Update original square's rank and file
		auto orig_rank = getSquareRank(orig);
		auto orig_file = getSquareFile(orig);
	}
}

void Pawn::afterApplied(GameState& g, Move const& m) const
{
	Direction dir = m.getDestination() - m.getOrigin();

	if (dir == DIR_NORTH * 2 || dir == DIR_SOUTH * 2) {
		Direction half_dir = dir / 2;
		g.setEnPassantPawn(m.getOrigin() + half_dir);
	} else {
		if (g.hasEnPassant()) {
			Square enpassant = g.getEnPassantPawn();
			if (enpassant == m.getDestination()) {
				if (getSquareRank(enpassant) == RK_3)
					enpassant += DIR_NORTH; // White pawn
				else
					enpassant += DIR_SOUTH; // Black pawn
				g.clearSquare(enpassant);
			}
		}
	}
}

Castling::Castling(Square rook) : rook(rook) {}

Square Castling::getRookSquare() const { return rook; }

bool Castling::isValid(GameState const& game)
{
	// Rook must be in one of the four corners of the board
	if (rook != SQ_A1 && rook != SQ_A8 && rook != SQ_H1 && rook != SQ_H8)
		return false;

	const auto& rook_piece = game.getPieceAt(rook);

	// There must be a rook at the given position
	if (rook_piece.getType()->getId() != PieceTypeId::ROOK)
		return false;

	bool white_rook = rook_piece.getColour() == Colour::WHITE;
	Square king = white_rook ? SQ_E1 : SQ_E8;

	const auto& king_piece = game.getPieceAt(king);

	// There must be a king at the given position
	if (king_piece.getType()->getId() != PieceTypeId::KING)
		return false;

	// Both pieces must have never been moved
	// This already implies they are of same colour
	if (game.wasSquareAltered(rook) || game.wasSquareAltered(king))
		return false;

	// Between the two pieces there must be no other piece
	Direction king_dir = (king < rook) ? DIR_EAST : DIR_WEST;
	for (Square sq = king + king_dir; sq != rook; sq += king_dir)
		if (!game.getPieceAt(sq).isClear())
			return false;

	return true;
}

void Castling::apply(GameState& game)
{
	const auto& rook_piece = game.getPieceAt(rook);
	bool white_rook = rook_piece.getColour() == Colour::WHITE;
	Square king = white_rook ? SQ_E1 : SQ_E8;

	Direction king_dir = (king < rook) ? DIR_EAST : DIR_WEST;

	Square king_dest = king + 2 * king_dir;
	Square rook_dest = king_dest - king_dir;

	Move(king, king_dest).apply(game);
	Move(rook, rook_dest).apply(game);
}