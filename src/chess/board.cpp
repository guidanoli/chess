#include "board.h"

#include "types.h"

// Places a piece of type t in rank r and file f in matrix m
// and mirrors it for white and black pieces
#define R_MIRROR(m, r, f, t)             \
do {                                     \
	m[r][f] = Piece(t, Colour::WHITE);  \
	m[~r][f] = Piece(t, Colour::BLACK); \
} while(0)

Board::Board()
{
	m_matrix = std::array<std::array<Piece, 8>, 8>();

	// First rank in order from left to right
	static PieceType first_rank[8] = {
		Rook{}, Knight{}, Bishop{}, Queen{}, King{}, Bishop{}, Knight{}, Rook{}
	};

	// First Rank
	for (File f = FL_A; f < FL_CNT; ++f)
		R_MIRROR(m_matrix, RK_1, f, first_rank[f]);

	// Second Rank
	for (File f = FL_A; f < FL_CNT; ++f)
		R_MIRROR(m_matrix, RK_2, f, Pawn{});
}

Piece& Board::operator[](Square sq)
{
	return m_matrix[getSquareRank(sq)][getSquareFile(sq)];
}

Piece const& Board::operator[](Square sq) const
{
	return m_matrix[getSquareRank(sq)][getSquareFile(sq)];
}