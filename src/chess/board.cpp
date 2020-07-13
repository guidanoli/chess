#include "board.h"

#include <assert.h>

#include "types.h"

using namespace std;
using namespace chesslib;

// Places a piece of type t in rank r and file f in matrix m
// and mirrors it for white and black pieces
#define R_MIRROR(m, r, f, t)             \
do {                                     \
	auto& white = m[getSquare(r, f)];    \
	auto& black = m[getSquare(~r, f)];   \
	white.setType(t);                    \
	white.setColour(Colour::WHITE);      \
	black.setType(t);                    \
	black.setColour(Colour::BLACK);      \
} while(0)

Board::Board()
{
	static const PieceTypeId first_rank_ids[FL_CNT] = {
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

optional<Square> Board::find(PieceTypeId piece_type_id,
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
	return nullopt;
}

Board::Board(Board const& board)
{ 
	copy(board.m_matrix, board.m_matrix + SQ_CNT, m_matrix);
}

Piece& Board::operator[](Square sq)
{
	assert(SquareCheck(sq));
	return m_matrix[sq];
}

Piece const& Board::operator[](Square sq) const
{
	assert(SquareCheck(sq));
	return m_matrix[sq];
}

void Board::pretty(ostream& os) const
{
	os << "    ";
	for (char fc = 'a'; fc <= 'h'; ++fc)
		os << fc << " ";
	os << endl << "   _";
	for (char fc = 'a'; fc <= 'h'; ++fc)
		os << "__";
	os << endl;
	for (Rank r = RK_8; r >= RK_1; --r) {
		os << r - RK_1 + 1 << " | ";
		for (File f = FL_A; f <= FL_H; ++f) {
			Square sq = getSquare(r, f);
			Piece p = operator[](sq);
			os << p << " ";
		}
		os << "|" << endl;
	}
	os << "   ";
	for (char fc = 'a'; fc <= 'h'; ++fc)
		os << "--";
	os << "-" << endl;
}
