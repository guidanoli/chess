#include "board.h"

#include <assert.h>

#include "types.h"

// Places a piece of type t in rank r and file f in matrix m
// and mirrors it for white and black pieces
#define R_MIRROR(m, r, f, t)                               \
do {                                                       \
	m[r][f].setType(getPieceTypeById(t));              \
	m[r][f].setColour(Colour::WHITE);                  \
	m[~r][f].setType(getPieceTypeById(t));             \
	m[~r][f].setColour(Colour::BLACK);                 \
} while(0)

Board::Board()
{
	static PieceTypeId first_rank_ids[8] = {
		PieceTypeId::ROOK,
		PieceTypeId::KNIGHT,
		PieceTypeId::BISHOP,
		PieceTypeId::QUEEN,
		PieceTypeId::KING,
		PieceTypeId::BISHOP,
		PieceTypeId::KNIGHT,
		PieceTypeId::ROOK,
	};

	// First Rank
	for (File f = FL_A; f < FL_CNT; ++f)
		R_MIRROR(m_matrix, RK_1, f, first_rank_ids[f]);

	// Second Rank
	for (File f = FL_A; f < FL_CNT; ++f)
		R_MIRROR(m_matrix, RK_2, f, PieceTypeId::PAWN);
}

std::optional<Square> Board::find(PieceTypeId piece_type_id,
                                  Colour colour) const
{
	assert(PieceTypeIdCheck(piece_type_id));
	assert(ColourCheck(colour));
	for (Square square = SQ_A1; square < SQ_CNT; ++square) {
		auto& p = (*this)[square];
		if (p.getType()->getId() == piece_type_id &&
			p.getColour() == colour)
			return square;
	}
	return std::nullopt;
}

Board::Board(Board const& board)
{ 
	std::copy(&board.m_matrix[0][0],
	          &board.m_matrix[0][0] + 64,
	          &m_matrix[0][0]);
}

Piece& Board::operator[](Square sq)
{
	assert(SquareCheck(sq));
	return m_matrix[getSquareRank(sq)][getSquareFile(sq)];
}

Piece const& Board::operator[](Square sq) const
{
	assert(SquareCheck(sq));
	return m_matrix[getSquareRank(sq)][getSquareFile(sq)];
}
