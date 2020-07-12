#pragma once

#include <memory>

class GameState;
enum Square;
class Piece;
enum class Colour;
enum class EnPassantPawn;

class GameStateControllerConst
{
public:
	Piece const& getPieceAt(Square sq) const;
	Colour getTurn() const;
	EnPassantPawn getEnPassantPawn() const;
	bool wasSquareAltered(Square sq) const;
public:
	GameStateControllerConst(GameStateControllerConst const&) = delete;
	GameStateControllerConst& operator=(GameStateControllerConst const&) = delete;
protected:
	GameStateControllerConst(std::shared_ptr<GameState const> game_ptr);
protected:
	std::shared_ptr<GameState const> game_ptr_const;
private:
	friend class GameState;
};

class GameStateController : public GameStateControllerConst
{
public:
	void movePiece(Square origin, Square dest);
	void clearSquare(Square sq);
	Piece& getPieceAt(Square sq);
	void setEnPassantPawn(EnPassantPawn pawn);
protected:
	GameStateController(std::shared_ptr<GameState> game_ptr);
protected:
	std::shared_ptr<GameState> game_ptr;
private:
	friend class GameState;
};

class DebugGameStateController : public GameStateController
{
public:
	void nextTurn();
	void setSquareAltered(Square sq, bool altered);
protected:
	DebugGameStateController(std::shared_ptr<GameState> game_ptr);
private:
	friend class GameState;
};