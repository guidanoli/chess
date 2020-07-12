#include "state.h"

#include <algorithm>
#include <cassert>
#include <vector>
#include <iostream>

#include "board.h"
#include "event.h"
#include "listener.h"
#include "types.h"
#include "error.h"
#include "controller.h"

class DummyGameListener : public GameListener
{
public:
	PieceTypeId promotePawn(GameState const& game, Square pawn) override;
	void catchError(GameState const& game, GameError err) override {};
};

PieceTypeId DummyGameListener::promotePawn(GameState const& game, Square pawn)
{
	return PieceTypeId::QUEEN;
}

GameState::GameState(std::shared_ptr<GameListener> listener) :
	m_turn(Colour::WHITE),
	m_enpassant_pawn(EnPassantPawn::NONE),
	m_listener(listener),
	m_phase(Phase::RUNNING)
{}

GameState::GameState(GameState const& game) :
	m_board(game.m_board),
	m_turn(game.m_turn),
	m_enpassant_pawn(game.m_enpassant_pawn),
	m_listener(game.m_listener),
	m_altered_squares(game.m_altered_squares),
	m_phase(game.m_phase)
{}

void GameState::setListener(std::shared_ptr<GameListener> listener)
{
	m_listener = listener;
}

bool GameState::inCheck(Colour c) const
{
	auto king_sq = getKingSquare(c);

	for (Square attacker_sq = SQ_A1; attacker_sq < SQ_CNT; ++attacker_sq) {
		Move attack(attacker_sq, king_sq);
		if (attack.isValidCheck(getConstController()))
			return true;
	}

	return false;
}

DebugGameStateController GameState::getDebugController()
{
	return DebugGameStateController(std::shared_ptr<GameState>(this));
}

GameStateController GameState::getController()
{
	return GameStateController(std::shared_ptr<GameState>(this));
}

GameStateControllerConst GameState::getConstController() const
{
	return GameStateControllerConst(std::shared_ptr<GameState const>(this));
}

Square GameState::getKingSquare(Colour c) const
{
	auto king_sq_opt = m_board.find(PieceTypeId::KING, c);
	assert(king_sq_opt); // all kings must be on the board
	return *king_sq_opt;
}

bool GameState::update(GameEvent* e)
{
	if (!canUpdate(e))
		return false;

	const EnPassantPawn ep_before = m_enpassant_pawn;

	e->apply(getController());

	if (ep_before == m_enpassant_pawn)
		setEnPassantPawn(EnPassantPawn::NONE);

	lookForPromotion();

	nextTurn();

	lookForCheckmate();

	return true;
}

void GameState::lookForPromotion()
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
					raiseError(GameError::ILLEGAL_PROMOTION);
				}
			}
			piece.setType(getPieceTypeById(new_type));
			return;
		}
	}
}

void GameState::lookForCheckmate()
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

bool GameState::canUpdate(GameEvent* e) const
{
	if (m_phase != Phase::RUNNING)
		return false;

	if (!e->isValid(getConstController()))
		return false;

	if (wouldEventCauseCheck(e))
		return false;

	return true;
}

bool GameState::wouldEventCauseCheck(GameEvent* e) const
{
	return simulate([e] (GameState& g) {
		e->apply(g.getController());
		auto turn = g.getTurn();
		g.nextTurn();
		return g.inCheck(turn);
	});
}

bool GameState::simulate(std::function<bool(GameState&)> cb) const
{
	auto copy = GameState(*this);
	copy.setListener(std::make_shared<DummyGameListener>());
	return cb(copy);
}

void GameState::nextTurn()
{
	m_turn = static_cast<Colour>(1 - static_cast<int>(m_turn));
}

Board& GameState::getBoard()
{
	return m_board;
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
	m_enpassant_pawn = pawn;
}

bool GameState::wasSquareAltered(Square sq) const
{
	auto it = m_altered_squares.find(sq);
	return it != m_altered_squares.end() && it->second;
}

void GameState::setSquareAltered(Square sq, bool altered)
{
	m_altered_squares[sq] = altered;
}

void GameState::raiseError(GameError err) const
{
	m_listener->catchError(*this, err);
}

void GameState::pretty(std::ostream& os)
{
	os << "    ";
	for (char fc = 'a'; fc <= 'h'; ++fc)
		os << fc << " ";
	os << std::endl << "   _";
	for (char fc = 'a'; fc <= 'h'; ++fc)
		os << "__";
	os << std::endl;
	for (Rank r = RK_8; r >= RK_1; --r) {
		os << r - RK_1 + 1 << " | ";
		for (File f = FL_A; f <= FL_H; ++f) {
			Square sq = getSquare(r, f);
			Piece p = getBoard()[sq];
			os << p << " ";
		}
		os << "|" << std::endl;
	}
	os << "   ";
	for (char fc = 'a'; fc <= 'h'; ++fc)
		os << "--";
	os << "-" << std::endl;
}

auto constexpr chesslib_version = 1;

#define failprefix "[ERROR] Chesslib: "

#define failandreturn(stream) do { \
	stream.setstate(std::ios::failbit); \
	return stream; \
} while(0)

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
		g.raiseError(GameError::IO_VERSION);
		failandreturn(in);
	}
	int turn;
	in >> turn;
	if (turn < 0 || turn > 1) {
		g.raiseError(GameError::IO_TURN);
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
			g.raiseError(GameError::IO_EN_PASSANT_FILE);
			failandreturn(in);
		}
		Rank r = getSquareRank(square);
		if (r != RK_3 && r != RK_6) {
			g.raiseError(GameError::IO_EN_PASSANT_RANK);
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
			g.raiseError(GameError::IO_SQUARE);
			failandreturn(in);
		}
		squares[sq] = true;
		auto& piece = g.m_board[square];
		int colour;
		in >> colour;
		if (colour < 0 || colour > 1) {
			g.raiseError(GameError::IO_COLOUR);
			failandreturn(in);
		}
		piece.setColour(static_cast<Colour>(colour));
		int type;
		in >> type;
		auto piece_type_id = static_cast<PieceTypeId>(type);
		if (!PieceTypeIdCheck(piece_type_id)) {
			g.raiseError(GameError::IO_PIECE_TYPE);
			failandreturn(in);
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

	g.lookForCheckmate();
	return in;
}
