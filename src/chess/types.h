#pragma once

#include <memory> // std::shared_ptr
#include <iostream> // std::istream, std::ostream

#include "defines.h" // macros

namespace chesslib
{

	class Move;
	class GameState;

	enum Square : int
	{
		SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
		SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
		SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
		SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
		SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
		SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
		SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
		SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
		SQ_CNT = 64,
	};

	enum Rank : int
	{
		RK_1, RK_2, RK_3, RK_4, RK_5, RK_6, RK_7, RK_8,
		RK_CNT = 8
	};

	enum File : int
	{
		FL_A, FL_B, FL_C, FL_D, FL_E, FL_F, FL_G, FL_H,
		FL_CNT = 8
	};

	inline Rank getSquareRank(Square sq)
	{
		return static_cast<Rank>(static_cast<int>(sq) >> 3);
	}
	
	inline File getSquareFile(Square sq)
	{
		return static_cast<File>(static_cast<int>(sq) & 0b111);
	}
	
	inline Square getSquare(Rank r, File f)
	{
		return static_cast<Square>(static_cast<int>(r) << 3 |
		                           static_cast<int>(f));
	}

	inline std::ostream& operator<<(std::ostream& out, Rank rk)
	{
		out << static_cast<int>(rk) + (1 - static_cast<int>(RK_1));
		return out;
	}

	inline std::ostream& operator<<(std::ostream& out, File fl)
	{
		out << static_cast<char>(static_cast<int>(fl) +
			(static_cast<int>('a') - static_cast<int>(FL_A)));
		return out;
	}

	inline std::ostream& operator<<(std::ostream& out, Square sq)
	{
		out << getSquareFile(sq) << getSquareRank(sq);
		return out;
	}

	enum Direction : int
	{
		DIR_NONE = 0,
		DIR_NORTH = RK_CNT,
		DIR_SOUTH = -DIR_NORTH,
		DIR_EAST = 1,
		DIR_WEST = -DIR_EAST,
		DIR_NORTHEAST = DIR_NORTH + DIR_EAST,
		DIR_NORTHWEST = DIR_NORTH + DIR_WEST,
		DIR_SOUTHEAST = DIR_SOUTH + DIR_EAST,
		DIR_SOUTHWEST = DIR_SOUTH + DIR_WEST,
	};

	enum class PieceTypeId
	{
		NONE = 0,
		PAWN = 1,
		KING = 2,
		QUEEN = 3,
		BISHOP = 4,
		KNIGHT = 5,
		ROOK = 6,
		MAX
	};

	enum class Colour
	{
		WHITE = 0,
		BLACK = 1,
		MAX
	};

	enum class Phase
	{
		RUNNING,
		WHITE_WON,
		BLACK_WON,
		MAX
	};

	class PieceType
	{
	public:
		PieceType(PieceTypeId id = PieceTypeId::NONE) : id(id) {}

		virtual bool canApply(GameState const& gameState, Move const& move) const { return false; }
		virtual void afterApplied(GameState& gameState, Move const& move) const {}
        
        PieceTypeId getId() const { return id; }
    private:
        PieceTypeId id;
	};

    class EmptyTile : public PieceType {};

	class Pawn : public PieceType
	{
	public:
		Pawn() : PieceType(PieceTypeId::PAWN) {}
		bool canApply(GameState const& gameState, Move const& move) const override;
		void afterApplied(GameState& gameState, Move const& move) const override;
	};

	class King : public PieceType
	{
	public:
		King() : PieceType(PieceTypeId::KING) {}
		bool canApply(GameState const& gameState, Move const& move) const override;
	};

	class Queen : public PieceType
	{
	public:
		Queen() : PieceType(PieceTypeId::QUEEN) {}
		bool canApply(GameState const& gameState, Move const& move) const override;
	};

	class Bishop : public PieceType
	{
	public:
		Bishop() : PieceType(PieceTypeId::BISHOP) {}
		bool canApply(GameState const& gameState, Move const& move) const override;
	};

	class Knight : public PieceType
	{
	public:
		Knight() : PieceType(PieceTypeId::KNIGHT) {}
		bool canApply(GameState const& gameState, Move const& move) const override;
	};

	class Rook : public PieceType
	{
	public:
		Rook() : PieceType(PieceTypeId::ROOK) {}
		bool canApply(GameState const& gameState, Move const& move) const override;
	};

	inline std::shared_ptr<PieceType> getPieceTypeById(PieceTypeId id)
	{
		static const std::shared_ptr<PieceType> piece_types[] = {
			std::make_shared<EmptyTile>(),
			std::make_shared<Pawn>(),
			std::make_shared<King>(),
			std::make_shared<Queen>(),
			std::make_shared<Bishop>(),
			std::make_shared<Knight>(),
			std::make_shared<Rook>()
		};
		return piece_types[static_cast<std::size_t>(id)];
	}

	class Piece
	{
	public:
		Piece() : c(Colour::WHITE) { clear(); }
		Piece(Piece const& p) : type(p.type), c(p.c) {}
		Piece(std::shared_ptr<PieceType> type, Colour c) : type(type), c(c) {}

		std::shared_ptr<PieceType> getType() const { return type; }
		Colour getColour() const { return c; }
		void setType(PieceTypeId id) { type = getPieceTypeById(id); }
		void setColour(Colour cl) { c = cl; }
		void clear() { setType(PieceTypeId::NONE); }
		bool isClear() const { return type->getId() == PieceTypeId::NONE; }

		Piece& operator=(Piece const& p)
		{
			type = p.type;
			c = p.c;
			return *this;
		}
	private:
		std::shared_ptr<PieceType> type;
		Colour c;
	};

	inline std::ostream& operator<<(std::ostream& out, Piece piece)
	{
		char c = piece.getColour() == Colour::WHITE ? 0 : ('A' - 'a');
		switch (piece.getType()->getId()) {
		case PieceTypeId::NONE:
			out << "_";
			return out;
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
			out << "?";
			return out;
		}
		out << c;
		return out;
	}

	ENABLE_BINARY_OPERATION_ON(Direction, +, Direction, Direction)
	ENABLE_BINARY_OPERATION_ON(Direction, -, Direction, Direction)
	ENABLE_COMUTATIVE_BINARY_OPERATION_ON(Direction, *, int, Direction)
	ENABLE_BINARY_OPERATION_ON(Direction, / , int, Direction)
	ENABLE_NEGATION_OPERATION_ON(Direction)
	ENABLE_ACTION_AND_ASSIGNMENT_OPERATION_ON(Direction, +, Direction)
	ENABLE_ACTION_AND_ASSIGNMENT_OPERATION_ON(Direction, -, Direction)

	ENABLE_COMUTATIVE_BINARY_OPERATION_ON(Square, +, Direction, Square)
	ENABLE_COMUTATIVE_BINARY_OPERATION_ON(Square, -, Direction, Square)
	ENABLE_ACTION_AND_ASSIGNMENT_OPERATION_ON(Square, +, Direction)
	ENABLE_ACTION_AND_ASSIGNMENT_OPERATION_ON(Square, -, Direction)

	ENABLE_BINARY_OPERATION_ON(Square, -, Square, Direction)

	ENABLE_INCR_OPERATORS_ON(Square)
	ENABLE_INCR_OPERATORS_ON(File)
	ENABLE_INCR_OPERATORS_ON(Rank)

	ENABLE_VALIDITY_CHECK(Square, SQ_CNT)
	ENABLE_VALIDITY_CHECK(File, FL_CNT)
	ENABLE_VALIDITY_CHECK(Rank, RK_CNT)
	ENABLE_VALIDITY_CHECK(Colour, Colour::MAX)
	ENABLE_VALIDITY_CHECK(PieceTypeId, PieceTypeId::MAX)
	ENABLE_VALIDITY_CHECK(Phase, Phase::MAX)

	ENABLE_MIRROR_OPERATOR_ON(Square, SQ_CNT)
	ENABLE_MIRROR_OPERATOR_ON(File, FL_CNT)
	ENABLE_MIRROR_OPERATOR_ON(Rank, RK_CNT)

	ENABLE_COMPARE_OPERATOR_ON(Square)

}
