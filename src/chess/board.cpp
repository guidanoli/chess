#include "board.h"

#include "types.h"

// Places a piece of type t in rank r and file f in matrix m
// and mirrors it for white and black pieces
#define R_MIRROR(m, r, f, t)                               \
do {                                                       \
	m[r][f] = Piece(getPieceTypeById(t), Colour::WHITE);   \
	m[~r][f] = Piece(getPieceTypeById(t), Colour::BLACK);  \
} while(0)

Board::Board()
{
	m_matrix = std::array<std::array<Piece, 8>, 8>();

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

Piece& Board::operator[](Square sq)
{
	return m_matrix[getSquareRank(sq)][getSquareFile(sq)];
}

Piece const& Board::operator[](Square sq) const
{
	return m_matrix[getSquareRank(sq)][getSquareFile(sq)];
}