#include "state.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "defines.h"
#include "error.h"

using namespace std;
using namespace chesslib;

GameState::GameState() :
	m_turn(Colour::WHITE),
	m_phase(Phase::RUNNING),
	m_enpassant_pawn(Square::SQ_CNT)
{
	fill(m_altered_map,
	     m_altered_map + size(m_altered_map),
	     false);
}

GameState::GameState(GameState const& other) :
	m_board(other.m_board),
	m_turn(other.m_turn),
	m_phase(other.m_phase),
	m_enpassant_pawn(other.m_enpassant_pawn)
{
	copy(other.m_altered_map,
	     other.m_altered_map + size(m_altered_map),
	     m_altered_map);
}

void GameState::nextTurn()
{
	m_turn = static_cast<Colour>(1 - static_cast<int>(m_turn));
}

Board& GameState::getBoard()
{
	return m_board;
}

void GameState::setPhase(Phase phase)
{
	assert(PhaseCheck(phase));
	m_phase = phase;
}

Phase GameState::getPhase() const
{
	return m_phase;
}

Board const& GameState::getBoard() const
{
	return m_board;
}

Colour GameState::getTurn() const
{
	return m_turn;
}

Square GameState::getEnPassantPawn() const
{
	return m_enpassant_pawn;
}

void GameState::setEnPassantPawn(Square pawn)
{
	assert(EnPassantPawnCheck(pawn));
	m_enpassant_pawn = pawn;
}

bool GameState::wasSquareAltered(Square sq) const
{
	assert(SquareCheck(sq));
	return m_altered_map[static_cast<size_t>(sq)];
}

void GameState::setSquareAltered(Square sq, bool altered)
{
	assert(SquareCheck(sq));
	m_altered_map[static_cast<size_t>(sq)] = altered;
}

void GameState::movePiece(Square origin, Square dest)
{
	assert(SquareCheck(origin));
	assert(SquareCheck(dest));

	auto& origpiece = getPieceAt(origin);
	auto& destpiece = getPieceAt(dest);

	destpiece = origpiece;
	origpiece.clear();

	setSquareAltered(origin, true);
	setSquareAltered(dest, true);
}

ostream& chesslib::operator<<(ostream& out, GameState const& g)
{
	out << chesslib::major_version << endl;
	out << static_cast<int>(g.m_turn) << endl;
	out << static_cast<int>(g.m_enpassant_pawn) << endl;
	for (Square square = SQ_A1; square < SQ_CNT; ++square) {
		const auto& p = g.m_board[square];
		const auto id = p.getType()->getId();
		const auto altered = g.wasSquareAltered(square);
		if (id == PieceTypeId::NONE)
			continue;
		out << static_cast<int>(square) << " ";
		out << static_cast<int>(p.getColour()) << " ";
		out << static_cast<int>(id) << " ";
		out << altered << endl;
	}
	out << -1;
	return out;
}

istream& chesslib::operator>>(istream& in, GameState& g)
{
	int version;
	in >> version;
	if (version != chesslib::major_version) {
		in.setstate(ios::failbit);
		throw GameError::IO_VERSION;
	}
	int turn_int;
	in >> turn_int;
	if (turn_int < 0 || turn_int > 1) {
		in.setstate(ios::failbit);
		throw GameError::IO_TURN;
	}
	g.m_turn = static_cast<Colour>(turn_int);
	int enpassant_int;
	in >> enpassant_int;
	auto enpassant = static_cast<Square>(enpassant_int);
	if (!EnPassantPawnCheck(enpassant)) {
		in.setstate(ios::failbit);
		throw GameError::IO_EN_PASSANT;
	}
	g.m_enpassant_pawn = enpassant;
	vector<bool> has_piece_map(64, false);
	int square_int;
	for (in >> square_int; square_int != -1; in >> square_int) {
		Square square = static_cast<Square>(square_int);
		if (!SquareCheck(square)) {
			in.setstate(ios::failbit);
			throw GameError::IO_SQUARE;
		}
		has_piece_map[square_int] = true;
		int colour_int;
		in >> colour_int;
		if (colour_int < 0 || colour_int > 1) {
			in.setstate(ios::failbit);
			throw GameError::IO_COLOUR;
		}
		auto& piece = g.m_board[square];
		piece.setColour(static_cast<Colour>(colour_int));
		int type_int;
		in >> type_int;
		auto type = static_cast<PieceTypeId>(type_int);
		if (!PieceTypeIdCheck(type)) {
			in.setstate(ios::failbit);
			throw GameError::IO_PIECE_TYPE;
		}
		piece.setType(type);
		bool altered;
		in >> altered;
		g.setSquareAltered(square, altered);
	}
	for (Square sq = SQ_A1; sq < SQ_CNT; ++sq)
		if (!has_piece_map[static_cast<size_t>(sq)])
			g.m_board[sq].clear();
	return in;
}

void GameState::clearEnPassantPawn()
{
	m_enpassant_pawn = Square::SQ_CNT;
}

bool GameState::hasEnPassant() const
{
	return m_enpassant_pawn != Square::SQ_CNT;
}

void GameState::clearSquare(Square sq)
{
	getPieceAt(sq).clear();
}

Piece const& GameState::getPieceAt(Square sq) const
{
	return getBoard()[sq];
}

Piece& GameState::getPieceAt(Square sq)
{
	return getBoard()[sq];
}