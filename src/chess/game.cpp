#include "game.h"

#include <algorithm>
#include <cassert>
#include <vector>
#include <iostream>

#include "board.h"
#include "event.h"

static void print_piece(Piece p);
static void print_chess_board(Board const& board);

class DummyGameListener : public GameListener
{
public:
	PieceTypeId promotePawn(Game const& game, Square pawn) override;
	void catchError(Game const& game, GameError err) override {};
};

PieceTypeId DummyGameListener::promotePawn(Game const& game, Square pawn)
{
	return PieceTypeId::QUEEN;
}

Game::Game(std::shared_ptr<GameListener> listener) :
	m_turn(Colour::WHITE),
	m_enpassant_pawn(EnPassantPawn::NONE),
	m_listener(listener),
	m_phase(Phase::RUNNING)
{}

Game::Game(Game const& game) :
	m_board(game.m_board),
	m_turn(game.m_turn),
	m_enpassant_pawn(game.m_enpassant_pawn),
	m_listener(game.m_listener),
	m_phase(game.m_phase)
{}

void Game::pretty() const
{
	print_chess_board(m_board);
}

void Game::setListener(std::shared_ptr<GameListener> listener)
{
	m_listener = listener;
}

bool Game::inCheck(Colour c) const
{
	auto king_sq = getKingSquare(c);

	for (Square attacker_sq = SQ_A1; attacker_sq < SQ_CNT; ++attacker_sq) {
		Move attack(attacker_sq, king_sq);
		if (attack.isValidCheck(*this))
			return true;
	}

	return false;
}

Square Game::getKingSquare(Colour c) const
{
	auto king_sq_opt = m_board.find(PieceTypeId::KING, c);
	assert(king_sq_opt); // all kings must be on the board
	return *king_sq_opt;
}

bool Game::canMove(Square origin, Square dest) const
{
	Move attack(origin, dest);
	return canUpdate(&attack);
}

bool Game::update(Event* e)
{
	if (!canUpdate(e))
		return false;

	const EnPassantPawn ep_before = m_enpassant_pawn;

	e->apply(*this);

	if (ep_before == m_enpassant_pawn)
		setEnPassantPawn(EnPassantPawn::NONE);

	lookForPromotion();

	nextTurn();

	lookForCheckmate();

	return true;
}

void Game::lookForPromotion()
{
	Rank last_rank = (getTurn() == Colour::WHITE) ? RK_8 : RK_1;
	for (File f = FL_A; f < FL_CNT; ++f) {
		Square sq = getSquare(last_rank, f);
		auto& piece = m_board[sq];
		if (*piece.getType() == PieceTypeId::PAWN) {
			PieceTypeId new_type;
			while(true) {

				new_type = m_listener->promotePawn(*this, sq);

				if (new_type != PieceTypeId::NONE &&
					new_type != PieceTypeId::PAWN &&
					new_type != PieceTypeId::KING)
				{
					break;
				}
				else
				{
					m_listener->catchError(*this, GameError::ILLEGAL_PROMOTION);
				}
			}
			piece.setType(getPieceTypeById(new_type));
			return;
		}
	}
}

void Game::lookForCheckmate()
{
	const Colour c = getTurn();
	for (Square piece_sq = SQ_A1; piece_sq < SQ_CNT; ++piece_sq) {
		auto const& p = m_board[piece_sq];
		if (p.getColour() != c)
			continue;
		for (Square dest_sq = SQ_A1; dest_sq < SQ_CNT; ++dest_sq) {
			Move move(piece_sq, dest_sq);
			if (canUpdate(&move))
				return;
		}
	}
	if (c == Colour::WHITE)
		m_phase = Phase::BLACK_WON;
	else
		m_phase = Phase::WHITE_WON;
}

bool Game::canUpdate(Event* e) const
{
	if (m_phase != Phase::RUNNING)
		return false;

	if (!e->isValid(*this))
		return false;

	if (wouldEventCauseCheck(e))
		return false;

	return true;
}

bool Game::wouldEventCauseCheck(Event* e) const
{
	return simulate([e] (Game& g) {
		e->apply(g);
		auto turn = g.getTurn();
		g.nextTurn();
		return g.inCheck(turn);
	});
}

bool Game::simulate(std::function<bool(Game&)> cb) const
{
	auto copy = Game(*this);
	copy.setListener(std::make_shared<DummyGameListener>());
	return cb(copy);
}

void Game::nextTurn()
{
	m_turn = Colour(1 - (int) m_turn);
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
	for (Square square = SQ_A1; square < SQ_CNT; ++square) {
		const auto& p = g.m_board[square];
		const auto id = p.getType()->getId();
		if (id == PieceTypeId::NONE)
			continue;
		fs << square << " ";
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
		Square square = (Square) enpassant;
		File f = getSquareFile(square);
		if (!FileCheck(f))
			failandreturn(fs);
		Rank r = getSquareRank(square);
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
		if (!PieceTypeIdCheck((PieceTypeId) type))
			failandreturn(fs);
		piece.setType(getPieceTypeById((PieceTypeId) type));
		fs >> sq;
	}
	for (int i = 0; i < 64; ++i)
		if (!squares[i])
			g.m_board[(Square) i].clear();
	g.lookForCheckmate();
	return fs;
}

static void print_piece(Piece p) {
	char c = p.getColour() == Colour::WHITE ? 0 : ('A' - 'a');
	switch (*p.getType()) {
	case PieceTypeId::NONE:
		std::cout << "_";
		return;
	case PieceTypeId::PAWN:
		c += 'p';
		break;
	case PieceTypeId::KING:
		c += 'k';
		break;
	case PieceTypeId::QUEEN:
		c += 'q';
		break;
	case PieceTypeId::BISHOP:
		c += 'b';
		break;
	case PieceTypeId::KNIGHT:
		c += 'n';
		break;
	case PieceTypeId::ROOK:
		c += 'r';
		break;
	default:
		std::cout << "?";
		return;
	}
	std::cout << c;
}

static void print_chess_board(Board const& board)
{
	std::cout << "    ";
	for (char fc = 'a'; fc <= 'h'; ++fc)
		std::cout << fc << " ";
	std::cout << std::endl << "   _";
	for (char fc = 'a'; fc <= 'h'; ++fc)
		std::cout << "__";
	std::cout << std::endl;
	for (Rank r = RK_8; r >= RK_1; --r) {
		std::cout << r - RK_1 + 1 << " | ";
		for (File f = FL_A; f <= FL_H; ++f) {
			Square sq = getSquare(r, f);
			Piece p = board[sq];
			print_piece(p);
			std::cout << " ";
		}
		std::cout << "|" << std::endl;
	}
	std::cout << "   ";
	for (char fc = 'a'; fc <= 'h'; ++fc)
		std::cout << "--";
	std::cout << "-" << std::endl;
}