#include "controller.h"

#include "board.h"
#include "state.h"

GameStateControllerConst::GameStateControllerConst(std::shared_ptr<GameState const> game_ptr) :
	game_ptr_const(game_ptr)
{}

GameStateController::GameStateController(std::shared_ptr<GameState> game_ptr) :
	GameStateControllerConst(game_ptr),
	game_ptr(game_ptr)
{}

Piece& GameStateController::getPieceAt(Square sq)
{
	return game_ptr->getBoard()[sq];
}

Piece const& GameStateControllerConst::getPieceAt(Square sq) const
{
	return game_ptr_const->getBoard()[sq];
}

Colour GameStateControllerConst::getTurn() const
{
	return game_ptr_const->getTurn();
}

void GameStateController::clearSquare(Square sq)
{
	getPieceAt(sq).clear();
}

EnPassantPawn GameStateControllerConst::getEnPassantPawn() const
{
	return game_ptr_const->getEnPassantPawn();
}

void GameStateController::setEnPassantPawn(EnPassantPawn pawn)
{
	game_ptr->setEnPassantPawn(pawn);
}

void GameStateController::movePiece(Square origin, Square dest)
{
	auto& origpiece = getPieceAt(origin);
	auto& destpiece = getPieceAt(dest);

	destpiece = origpiece;
	origpiece.clear();

	game_ptr->setSquareAltered(origin, true);
	game_ptr->setSquareAltered(dest, true);
}

bool GameStateControllerConst::wasSquareAltered(Square sq) const
{
	return game_ptr_const->wasSquareAltered(sq);
}

DebugGameStateController::DebugGameStateController(std::shared_ptr<GameState> game_ptr) :
	GameStateController(game_ptr)
{}

void DebugGameStateController::nextTurn()
{
	game_ptr->nextTurn();
}

void DebugGameStateController::setSquareAltered(Square sq, bool altered)
{
	game_ptr->setSquareAltered(sq, altered);
}