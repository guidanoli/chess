#include "state.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "error.h"

GameState::GameState() :
	m_turn(Colour::WHITE),
	m_phase(Phase::RUNNING),
	m_enpassant_pawn(EnPassantPawn::NONE)
{
	std::fill(m_altered_map,
	          m_altered_map + std::size(m_altered_map),
	          false);
}

GameState::GameState(GameState const& other) :
	m_board(other.m_board),
	m_turn(other.m_turn),
	m_phase(other.m_phase),
	m_enpassant_pawn(other.m_enpassant_pawn)
{
	std::copy(other.m_altered_map,
	          other.m_altered_map + std::size(m_altered_map),
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

EnPassantPawn GameState::getEnPassantPawn() const
{
	return m_enpassant_pawn;
}

void GameState::setEnPassantPawn(EnPassantPawn pawn)
{
	assert(EnPassantPawnCheck(pawn));
	m_enpassant_pawn = pawn;
}

bool GameState::wasSquareAltered(Square sq) const
{
	assert(SquareCheck(sq));
	return m_altered_map[static_cast<std::size_t>(sq)];
}

void GameState::setSquareAltered(Square sq, bool altered)
{
	assert(SquareCheck(sq));
	m_altered_map[static_cast<std::size_t>(sq)] = altered;
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

auto constexpr chesslib_version = 1;

std::ostream& operator<<(std::ostream& out, GameState const& g)
{
	out << chesslib_version << std::endl;
	out << static_cast<int>(g.m_turn) << std::endl;
	out << static_cast<int>(g.m_enpassant_pawn) << std::endl;
	for (Square square = SQ_A1; square < SQ_CNT; ++square) {
		const auto& p = g.m_board[square];
		const auto id = p.getType()->getId();
		const auto altered = g.wasSquareAltered(square);
		if (id == PieceTypeId::NONE)
			continue;
		out << static_cast<int>(square) << " ";
		out << static_cast<int>(p.getColour()) << " ";
		out << static_cast<int>(id) << " ";
		out << altered << std::endl;
	}
	out << -1;
	return out;
}

std::istream& operator>>(std::istream& in, GameState& g)
{
	int version;
	in >> version;
	if (version != chesslib_version) {
		in.setstate(std::ios::failbit);
		throw GameError::IO_VERSION;
	}
	int turn;
	in >> turn;
	if (turn < 0 || turn > 1) {
		in.setstate(std::ios::failbit);
		throw GameError::IO_TURN;
	}
	g.m_turn = static_cast<Colour>(turn);
	int enpassant;
	in >> enpassant;
	auto en_passant_pawn = static_cast<EnPassantPawn>(enpassant);
	if (en_passant_pawn != EnPassantPawn::NONE) {
		Square square = static_cast<Square>(enpassant);
		File f = getSquareFile(square);
		if (!FileCheck(f)) {
			in.setstate(std::ios::failbit);
			throw GameError::IO_EN_PASSANT_FILE;
		}
		Rank r = getSquareRank(square);
		if (r != RK_3 && r != RK_6) {
			in.setstate(std::ios::failbit);
			throw GameError::IO_EN_PASSANT_RANK;
		}
	}
	g.m_enpassant_pawn = static_cast<EnPassantPawn>(enpassant);
	int sq;
	in >> sq;
	std::vector<bool> squares(64, false);
	while (sq != -1) {
		Square square = static_cast<Square>(sq);
		if (!SquareCheck(square)) {
			in.setstate(std::ios::failbit);
			throw GameError::IO_SQUARE;
		}
		squares[sq] = true;
		auto& piece = g.m_board[square];
		int colour;
		in >> colour;
		if (colour < 0 || colour > 1) {
			in.setstate(std::ios::failbit);
			throw GameError::IO_COLOUR;
		}
		piece.setColour(static_cast<Colour>(colour));
		int type;
		in >> type;
		auto piece_type_id = static_cast<PieceTypeId>(type);
		if (!PieceTypeIdCheck(piece_type_id)) {
			in.setstate(std::ios::failbit);
			throw GameError::IO_PIECE_TYPE;
		}
		piece.setType(getPieceTypeById(piece_type_id));
		bool altered;
		in >> altered;
		g.setSquareAltered(square, altered);
		in >> sq;
	}
	for (Square sq = SQ_A1; sq < SQ_CNT; ++sq)
		if (!squares[static_cast<std::size_t>(sq)])
			g.m_board[sq].clear();
	return in;
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