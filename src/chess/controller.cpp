#include "controller.h"

#include <cassert>

#include "error.h"
#include "event.h"
#include "listener.h"
#include "state.h"

class DummyGameListener : public GameListener
{
	PieceTypeId promotePawn(GameController const& gameController,
	                        Square pawn) override
	{
		return PieceTypeId::QUEEN;
	}

	void catchError(GameController const& gameController,
	                GameError err) override {}
};

GameController::GameController(std::unique_ptr<GameState> gameStatePtr,
                               std::shared_ptr<GameListener> listener) :
	m_state(std::move(gameStatePtr)),
	m_listener(listener)
{}

GameController::GameController(GameController const& other) :
	m_state(std::make_unique<GameState>(*other.m_state)),
	m_listener(m_listener)
{}

GameState const& GameController::getState() const
{
	return *m_state;
}

void GameController::clearListener()
{
	m_listener = std::make_shared<DummyGameListener>();
}

bool GameController::inCheck(Colour c) const
{
	assert(ColourCheck(c));
	auto king_sq = getKingSquare(c);

	for (Square attacker_sq = SQ_A1; attacker_sq < SQ_CNT; ++attacker_sq) {
		Move attack(attacker_sq, king_sq);
		if (attack.isValidCheck(*m_state))
			return true;
	}

	return false;
}

Square GameController::getKingSquare(Colour c) const
{
	assert(ColourCheck(c));
	auto king_sq_opt = m_state->getBoard().find(PieceTypeId::KING, c);
	assert(king_sq_opt); // all kings must be on the board
	return *king_sq_opt;
}

bool GameController::update(std::shared_ptr<GameEvent> e)
{
	if (!canUpdate(e))
		return false;

	const EnPassantPawn ep_before = m_state->getEnPassantPawn();

	e->apply(*m_state);

	if (ep_before == m_state->getEnPassantPawn())
		m_state->setEnPassantPawn(EnPassantPawn::NONE);

	lookForPromotion();

	m_state->nextTurn();

	lookForCheckmate();

	return true;
}

void GameController::lookForPromotion()
{
	Rank last_rank = (m_state->getTurn() == Colour::WHITE) ? RK_8 : RK_1;
	for (File f = FL_A; f < FL_CNT; ++f) {
		Square sq = getSquare(last_rank, f);
		auto& piece = m_state->getPieceAt(sq);
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

void GameController::lookForCheckmate()
{
	const Colour c = m_state->getTurn();
	for (Square piece_sq = SQ_A1; piece_sq < SQ_CNT; ++piece_sq) {
		auto const& p = m_state->getPieceAt(piece_sq);
		if (p.getColour() != c)
			continue;
		for (Square dest_sq = SQ_A1; dest_sq < SQ_CNT; ++dest_sq) {
			auto move = std::make_shared<Move>(piece_sq, dest_sq);
			if (canUpdate(move))
				return;
		}
	}
	if (c == Colour::WHITE)
		m_state->setPhase(Phase::BLACK_WON);
	else
		m_state->setPhase(Phase::WHITE_WON);
}

bool GameController::canUpdate(std::shared_ptr<GameEvent> e) const
{
	if (m_state->getPhase() != Phase::RUNNING)
		return false;

	if (!e->isValid(*m_state))
		return false;

	if (wouldEventCauseCheck(e))
		return false;

	return true;
}

bool GameController::wouldEventCauseCheck(std::shared_ptr<GameEvent> e) const
{
	return simulate([e] (auto g) {
		e->apply(*g.m_state);
		auto turn = g.m_state->getTurn();
		g.m_state->nextTurn();
		return g.inCheck(turn);
	});
}

bool GameController::simulate(simulationCallback cb) const
{
	auto copy = GameController(*this);
	copy.clearListener();
	return cb(copy);
}

bool GameController::loadState(std::istream& is)
{
	try
	{
		is >> *m_state;
		lookForCheckmate();
		return true;
	}
	catch (GameError err)
	{
		raiseError(err);
		is.clear(std::ios::failbit);
		return false;
	}
}

bool GameController::saveState(std::ostream& os) const
{
	os << *m_state;
	return true;
}

void GameController::raiseError(GameError err) const
{
	m_listener->catchError(*this, err);
}