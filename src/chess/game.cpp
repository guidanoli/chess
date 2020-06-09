#include "game.h"

#include "board.h"
#include "event.h"

Game::Game() :
	m_turn(Colour::WHITE),
	m_enpassant_pawn(EnPassantPawn::NONE)
{}

bool Game::update(Event& e)
{
	if (!e.isValid(*this))
		return false;

	const EnPassantPawn enpassant_before = m_enpassant_pawn;

	e(*this);

	if (enpassant_before == m_enpassant_pawn)
		setEnPassantPawn(EnPassantPawn::NONE);

	return true;
}

Board& Game::getBoard()
{
	return m_board;
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