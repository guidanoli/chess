#include "board.h"

#include "types.h"

#define R_MIRROR(m, r, k) m[r][f] = m[~r][f]
#define F_MIRROR(m, r, k) m[r][f] = m[r][~f]

Board::Board()
{
	m_matrix = std::array<std::array<Piece, 8>, 8>();

	// First rank in order from left to right
	static Piece first_rank[8] = {
		Piece::ROOK, Piece::KNIGHT, Piece::BISHOP, Piece::QUEEN,
		Piece::KING, Piece::BISHOP, Piece::KNIGHT, Piece::ROOK
	};

	// First Rank
	for (File f = FL_A; f < FL_CNT; ++f)
		R_MIRROR(m_matrix, RK_1, f) = first_rank[f];

	// Second Rank
	for (File f = FL_A; f < FL_CNT; ++f)
		R_MIRROR(m_matrix, RK_2, f) = Piece::PAWN;

	// Third and Fourth Rank
	for (Rank r = RK_3; r <= RK_4; ++r)
		for (File f = FL_A; f < FL_CNT; ++f)
			R_MIRROR(m_matrix, r, f) = Piece::NONE;
}

Piece& Board::operator[](Square sq)
{
	return m_matrix[getSquareRank(sq)][getSquareFile(sq)];
}