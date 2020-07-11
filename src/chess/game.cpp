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

Game::Game(std::shared_ptr<GameListener> listener, bool debug) :
	m_debug(debug),
	m_turn(Colour::WHITE),
	m_enpassant_pawn(EnPassantPawn::NONE),
	m_listener(listener),
	m_phase(Phase::RUNNING)
{}

Game::Game(Game const& game) :
	m_debug(game.m_debug),
	m_board(game.m_board),
	m_turn(game.m_turn),
	m_enpassant_pawn(game.m_enpassant_pawn),
	m_listener(game.m_listener),
	m_altered_squares(game.m_altered_squares),
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

bool Game::isDebug() const
{
	return m_debug;
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

bool Game::update(Event* e)
{
	if (!canUpdate(e))
		return false;

	const EnPassantPawn ep_before = m_enpassant_pawn;

	e->apply(*this);

	if (ep_before == m_enpassant_pawn)
		privateSetEnPassantPawn(EnPassantPawn::NONE);

	lookForPromotion();

	privateNextTurn();

	lookForCheckmate();

	return true;
}

void Game::lookForPromotion()
{
	Rank last_rank = (getTurn() == Colour::WHITE) ? RK_8 : RK_1;
	for (File f = FL_A; f < FL_CNT; ++f) {
		Square sq = getSquare(last_rank, f);
		auto& piece = m_board[sq];
		if (piece.getType()->getId() == PieceTypeId::PAWN) {
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
		g.privateNextTurn();
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
	if (m_debug)
		privateNextTurn();
}

void Game::privateNextTurn()
{
	m_turn = static_cast<Colour>(1 - static_cast<int>(m_turn));
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
	if (m_debug)
		privateSetEnPassantPawn(pawn);
}

void Game::privateSetEnPassantPawn(EnPassantPawn pawn)
{
	m_enpassant_pawn = pawn;
}

bool Game::wasSquareAltered(Square sq) const
{
	auto it = m_altered_squares.find(sq);
	return it != m_altered_squares.end() && it->second;
}

void Game::setSquareAltered(Square sq, bool altered)
{
	if (m_debug)
		privateSetSquareAltered(sq, altered);
}

void Game::privateSetSquareAltered(Square sq, bool altered)
{
	m_altered_squares[sq] = altered;
}

auto constexpr chesslib_version = 1;

#define failprefix "[ERROR] Chesslib: "

#define failandreturn(stream) do { \
	stream.setstate(std::ios::failbit); \
	return stream; \
} while(0)

std::ostream& operator<<(std::ostream& out, Game const& g)
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

std::istream& operator>>(std::istream& in, Game& g)
{
	int version;
	in >> version;
	if (version != chesslib_version) {
		std::cerr << failprefix "Incompatible version " << version
		          << " (current is " << chesslib_version << ")" << std::endl;
		failandreturn(in);
	}
	int turn;
	in >> turn;
	if (turn < 0 || turn > 1) {
		std::cerr << failprefix "Invalid turn " << turn << std::endl;
		failandreturn(in);
	}
	g.m_turn = static_cast<Colour>(turn);
	int enpassant;
	in >> enpassant;
	auto en_passant_pawn = static_cast<EnPassantPawn>(enpassant);
	if (en_passant_pawn != EnPassantPawn::NONE) {
		Square square = static_cast<Square>(enpassant);
		File f = getSquareFile(square);
		if (!FileCheck(f)) {
			std::cerr << failprefix "Invalid enpassant file " << f << std::endl;
			failandreturn(in);
		}
		Rank r = getSquareRank(square);
		if (r != RK_3 && r != RK_6) {
			std::cerr << failprefix "Invalid enpassant rank " << r << std::endl;
			failandreturn(in);
		}
	}
	g.m_enpassant_pawn = static_cast<EnPassantPawn>(enpassant);
	int sq;
	in >> sq;
	std::vector<bool> squares(64, false);
	while (sq != -1) {
		Square square = static_cast<Square>(sq);
		if (!SquareCheck(square)) {
			std::cerr << failprefix "Invalid square " << square << std::endl;
			failandreturn(in);
		}
		squares[sq] = true;
		auto& piece = g.m_board[square];
		int colour;
		in >> colour;
		if (colour < 0 || colour > 1) {
			std::cerr << failprefix "Invalid colour " << colour << std::endl;
			failandreturn(in);
		}
		piece.setColour(static_cast<Colour>(colour));
		int type;
		in >> type;
		auto piece_type_id = static_cast<PieceTypeId>(type);
		if (!PieceTypeIdCheck(piece_type_id)) {
			std::cerr << failprefix "Invalid piece type" << type << std::endl;
			failandreturn(in);
		}
		piece.setType(getPieceTypeById(piece_type_id));
		bool altered;
		in >> altered;
		g.privateSetSquareAltered(square, altered);
		in >> sq;
	}
	for (Square sq = SQ_A1; sq < SQ_CNT; ++sq)
		if (!squares[static_cast<std::size_t>(sq)])
			g.m_board[sq].clear();
	g.lookForCheckmate();
	return in;
}

static void print_piece(Piece p) {
	char c = p.getColour() == Colour::WHITE ? 0 : ('A' - 'a');
	switch (p.getType()->getId()) {
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