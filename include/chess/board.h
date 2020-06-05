#pragma once

#include <array>

enum Square;
enum class Piece;

class Board
{
public:
	Board();
	Piece& operator[](Square sq);
private:
	std::array<std::array<Piece, 8>, 8> m_matrix;
};