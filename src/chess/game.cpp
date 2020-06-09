#include "game.h"

#include <vector>

#include "board.h"
#include "event.h"

Game::Game() :
	m_turn(Colour::WHITE),
	m_phase(Phase::RUNNING),
	m_enpassant_pawn(EnPassantPawn::NONE)
{}

bool Game::update(Event* e)
{
	if (!e->isValid(*this))
		return false;

	const EnPassantPawn enpassant_before = m_enpassant_pawn;

	e->operator()(*this);

	if (enpassant_before == m_enpassant_pawn)
		setEnPassantPawn(EnPassantPawn::NONE);

	m_turn = Colour(1 - (int) m_turn);

	return true;
}

Board& Game::getBoard()
{
	return m_board;
}

Phase Game::getPhase() const
{
	return m_phase;
}

Board const& Game::getBoard() const
{
	return m_board;
}

Colour Game::getTurn() const
{
	return m_turn;
}

EnPassantPawn Game::getEnPassantPawn() const
{
	return m_enpassant_pawn;
}

void Game::setEnPassantPawn(EnPassantPawn pawn)
{
	m_enpassant_pawn = pawn;
}

#define VERSION 0

std::ofstream& operator<<(std::ofstream& fs, Game const& g)
{
	fs << VERSION << std::endl;
	fs << (int) g.m_turn << std::endl;
	fs << (int) g.m_enpassant_pawn << std::endl;
	for (Square sq = SQ_A1; sq < SQ_CNT; ++sq) {
		const auto& p = g.m_board[sq];
		const auto id = p.getType()->getId();
		if (id == PieceTypeId::NONE)
			continue;
		fs << sq << " ";
		fs << (int) p.getColour() << " ";
		fs << (int) id << std::endl;
	}
	fs << -1;
	return fs;
}

#define failandreturn(fs) do { \
	fs.setstate(std::ios::failbit); \
	return fs; \
} while(0)

std::ifstream& operator>>(std::ifstream& fs, Game& g)
{
	int version;
	fs >> version;
	if (version != VERSION)
		failandreturn(fs);
	int turn;
	fs >> turn;
	if (turn < 0 || turn > 1)
		failandreturn(fs);
	g.m_turn = Colour(turn);
	int enpassant;
	fs >> enpassant;
	if (enpassant != (int) EnPassantPawn::NONE) {
		Square sq = (Square) enpassant;
		File f = getSquareFile(sq);
		if (!FileCheck(f))
			failandreturn(fs);
		Rank r = getSquareRank(sq);
		if (r != RK_3 && r != RK_6)
			failandreturn(fs);
	}
	g.m_enpassant_pawn = (EnPassantPawn) enpassant;
	int sq;
	fs >> sq;
	std::vector<bool> squares(64, false);
	while (sq != -1) {
		Square square = (Square) sq;
		if (!SquareCheck(square))
			failandreturn(fs);
		squares[sq] = true;
		auto& piece = g.m_board[square];
		int colour;
		fs >> colour;
		if (colour < 0 || colour > 1)
			failandreturn(fs);
		piece.setColour((Colour) colour);
		int type;
		fs >> type;
		if (type < 0 || type >= (int) PieceTypeId::MAX)
			failandreturn(fs);
		piece.setType(getPieceTypeById((PieceTypeId) type));
		fs >> sq;
	}
	for (int i = 0; i < 64; ++i)
		if (!squares[i])
			g.m_board[(Square) i].setType(
				getPieceTypeById(PieceTypeId::NONE));
	return fs;
}