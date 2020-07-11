#pragma once

#include <memory>

#if defined(_MSC_VER)
#pragma warning(disable: 26812) // Unescoped enum type
#endif

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

enum Direction : int
{
	DIR_NONE      = 0,
	DIR_NORTH     = 8,
	DIR_SOUTH     = -DIR_NORTH,
	DIR_EAST      = 1,
	DIR_WEST      = -DIR_EAST,
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

class Game;
class Move;

class PieceType
{
public:
	PieceType(PieceTypeId id = PieceTypeId::NONE) : id(id) {}

	virtual bool canApply(Game const& g, Move const& m) const { return false; }
	virtual void afterApplied(Game& g, Move const& m) const {}

	PieceTypeId getId() const { return id; }
	operator PieceTypeId() const { return getId(); }
private:
	PieceTypeId id;
};

class EmptyTile : public PieceType {};

class Pawn : public PieceType
{
public:
	Pawn() : PieceType(PieceTypeId::PAWN) {}
	bool canApply(Game const& g, Move const& m) const override;
	void afterApplied(Game& g, Move const& m) const override;
};

class King : public PieceType
{
public:
	King() : PieceType(PieceTypeId::KING) {}
	bool canApply(Game const& g, Move const& m) const override;
};

class Queen : public PieceType
{
public:
	Queen() : PieceType(PieceTypeId::QUEEN) {}
	bool canApply(Game const& g, Move const& m) const override;
};

class Bishop : public PieceType
{
public:
	Bishop() : PieceType(PieceTypeId::BISHOP) {}
	bool canApply(Game const& g, Move const& m) const override;
};

class Knight : public PieceType
{
public:
	Knight() : PieceType(PieceTypeId::KNIGHT) {}
	bool canApply(Game const& g, Move const& m) const override;
};

class Rook : public PieceType
{
public:
	Rook() : PieceType(PieceTypeId::ROOK) {}
	bool canApply(Game const& g, Move const& m) const override;
};

class Piece
{
public:
	Piece() : c(Colour::WHITE) { clear(); }
	Piece(Piece const& p) :
		type(p.type), c(p.c) {}
	Piece(std::shared_ptr<PieceType> type, Colour c) :
		type(type), c(c) {}

	std::shared_ptr<PieceType> getType() const { return type; }
	Colour getColour() const { return c; }
	void setType(std::shared_ptr<PieceType> t) { type = t; }
	void setColour(Colour cl) { c = cl; }
	void clear();

	Piece& operator=(Piece& p) {
		type = p.type;
		c = p.c;
		return *this;
	}
private:
	std::shared_ptr<PieceType> type;
	Colour c;
};

enum CastlingRookRights : int
{
	MOVED_NOTHING          = 0,
	MOVED_WHITE_LEFT_ROOK  = 1 << 0,
	MOVED_WHITE_RIGHT_ROOK = 1 << 1,
	MOVED_BLACK_LEFT_ROOK  = 1 << 2,
	MOVED_BLACK_RIGHT_ROOK = 1 << 3,
	MOVED_WHITE_KING       = 1 << 4,
	MOVED_BLACK_KING       = 1 << 5,
};

enum class CastlingRook
{
	WHITE_LEFT  = SQ_A1,
	WHITE_RIGHT = SQ_H1,
	BLACK_LEFT  = SQ_H8,
	BLACK_RIGHT = SQ_A8,
};

enum class EnPassantPawn
{
	NONE     = SQ_CNT,
	BLACK_A6 = SQ_A6,
	BLACK_B6 = SQ_B6,
	BLACK_C6 = SQ_C6,
	BLACK_D6 = SQ_D6,
	BLACK_E6 = SQ_E6,
	BLACK_F6 = SQ_F6,
	BLACK_G6 = SQ_G6,
	BLACK_H6 = SQ_H6,
	WHITE_A3 = SQ_A3,
	WHITE_B3 = SQ_B3,
	WHITE_C3 = SQ_C3,
	WHITE_D3 = SQ_D3,
	WHITE_E3 = SQ_E3,
	WHITE_F3 = SQ_F3,
	WHITE_G3 = SQ_G3,
	WHITE_H3 = SQ_H3,
};

enum class Phase
{
	RUNNING,
	WHITE_WON,
	BLACK_WON,
	MAX
};

#define ENABLE_BINARY_OP(T, op) \
constexpr T operator ## op(T d1, T d2) { return static_cast<T>(static_cast<int>(d1) op static_cast<int>(d2)); }	\

// Macros for defining extra base operations on enumerations
// From Stockfish source code

#define ENABLE_BASE_OPERATORS_ON(T)												\
ENABLE_BINARY_OP(T, +)															\
ENABLE_BINARY_OP(T, -)															\
constexpr T operator-(T d) { return static_cast<T>(-static_cast<int>(d)); }		\
inline T& operator+=(T& d1, T d2) { return d1 = d1 + d2; }						\
inline T& operator-=(T& d1, T d2) { return d1 = d1 - d2; }

#define ENABLE_INCR_OPERATORS_ON(T)													\
inline T& operator++(T& d) { return d = static_cast<T>(static_cast<int>(d) + 1); }	\
inline T& operator--(T& d) { return d = static_cast<T>(static_cast<int>(d) - 1); }

// End of external code

// Check if enum value is valid
#define ENABLE_VALIDITY_CHECK(T, MAX) \
inline constexpr bool T ## Check (T const& d) { return static_cast<int>(d) >= 0 && static_cast<int>(d) < static_cast<int>(MAX); }

#define ENABLE_MIRROR_OPERATOR_ON(T, MAX) \
inline constexpr T operator~(T d) { return static_cast<T>(static_cast<int>(MAX) - 1 - static_cast<int>(d)); }

#define ENABLE_COMPARE_OPERATOR_ON(T) \
ENABLE_BINARY_OP(T, >) \
ENABLE_BINARY_OP(T, <) \
ENABLE_BINARY_OP(T, >=) \
ENABLE_BINARY_OP(T, <=)

ENABLE_BASE_OPERATORS_ON(Direction)

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

#undef ENABLE_COMPARE_OPERATOR_ON
#undef ENABLE_MIRROR_OPERATOR_ON
#undef ENABLE_VALIDITY_CHECK
#undef ENABLE_INCR_OPERATORS_ON
#undef ENABLE_BASE_OPERATORS_ON
#undef ENABLE_BINARY_OP

inline Rank getSquareRank(Square sq) { return static_cast<Rank>(static_cast<int>(sq) >> 3); }
inline File getSquareFile(Square sq) { return static_cast<File>(static_cast<int>(sq) & 0b111); }
inline Square getSquare(Rank r, File f) { return static_cast<Square>(static_cast<int>(r) << 3 | static_cast<int>(f)); }

inline Direction operator-(Square dest, Square origin) {
	return static_cast<Direction>(static_cast<int>(dest) - static_cast<int>(origin));
}

inline Square& operator+=(Square& sq, Direction dir) {
	return sq = static_cast<Square>(static_cast<int>(sq) + static_cast<int>(dir));
}

inline Square operator+(Square sq, Direction dir) {
	return static_cast<Square>(static_cast<int>(sq) + static_cast<int>(dir));
}

inline Square& operator-=(Square& sq, Direction dir) {
	return sq = static_cast<Square>(static_cast<int>(sq) - static_cast<int>(dir));
}

inline Square operator-(Square sq, Direction dir) {
	return static_cast<Square>(static_cast<int>(sq) - static_cast<int>(dir));
}

inline Direction operator*(int n, Direction dir) {
	return static_cast<Direction>(n * static_cast<int>(dir));
}

inline Direction operator*(Direction dir, int n) {
	return static_cast<Direction>(n * static_cast<int>(dir));
}

inline Square getEnPassantPawnSquare(EnPassantPawn pawn) {
	return static_cast<Square>(static_cast<int>(pawn));
}

inline EnPassantPawn square2EnPassant(Square sq) {
	return static_cast<EnPassantPawn>(static_cast<int>(sq));
}

inline std::shared_ptr<PieceType> getPieceTypeById(PieceTypeId id) {
	static std::shared_ptr<PieceType> piece_types[] = {
		std::make_shared<EmptyTile>(),
		std::make_shared<Pawn>(),
		std::make_shared<King>(),
		std::make_shared<Queen>(),
		std::make_shared<Bishop>(),
		std::make_shared<Knight>(),
		std::make_shared<Rook>()
	};
	return piece_types[static_cast<int>(id)];
}

inline void Piece::clear() { type = getPieceTypeById(PieceTypeId::NONE); }
