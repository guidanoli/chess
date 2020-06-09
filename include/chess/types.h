#pragma once

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
	PAWN = 1 << 1,
	KING = 2 << 1,
	QUEEN = 3 << 1,
	BISHOP = 4 << 1,
	KNIGHT = 5 << 1,
	ROOK = 6 << 1
};

enum class Colour
{
	WHITE = 0,
	BLACK = 1,
};

class Game;
class Move;

struct PieceType
{
	virtual bool canApply(Game const& g, Move const& m) const { return false; }
	virtual void afterApplied(Game& g, Move const& m) const { }

	virtual PieceTypeId getId() const { return PieceTypeId::NONE; }
	operator PieceTypeId() const { return getId(); }
	friend bool operator==(PieceType const& a, PieceType const& b) {
		return a.getId() == b.getId();
	}
};

struct EmptyTile : PieceType {};

struct Pawn : PieceType {
	bool canApply(Game const& g, Move const& m) const;
	void afterApplied(Game& g, Move const& m) const;
	PieceTypeId getId() const { return PieceTypeId::PAWN; }
};

struct King : PieceType
{
	bool canApply(Game const& g, Move const& m) const;
	PieceTypeId getId() const { return PieceTypeId::KING; }
};

struct Queen : PieceType
{
	bool canApply(Game const& g, Move const& m) const;
	PieceTypeId getId() const { return PieceTypeId::QUEEN; }
};

struct Bishop : PieceType
{
	bool canApply(Game const& g, Move const& m) const;
	PieceTypeId getId() const { return PieceTypeId::BISHOP; }
};

struct Knight : PieceType
{
	bool canApply(Game const& g, Move const& m) const;
	PieceTypeId getId() const { return PieceTypeId::KNIGHT; }
};

struct Rook : PieceType
{
	bool canApply(Game const& g, Move const& m) const;
	PieceTypeId getId() const { return PieceTypeId::ROOK; }
};

class Piece
{
public:
	Piece() :
		type(EmptyTile{}), c(Colour::WHITE) {}
	Piece(PieceType type, Colour c) :
		type(type), c(c) {}
	Piece(Piece const& p) :
		type(p.type), c(p.c) {}
	PieceType getType() const { return type; }
	Colour getColour() const { return c; }
	void setType(PieceType& t) { type = t; }
	void setColour(Colour cl) { c = cl; }
	Piece& operator=(Piece& p) {
		type = p.type;
		c = p.c;
		return *this;
	}
private:
	PieceType type;
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
	WHITE_A6 = SQ_A6,
	WHITE_B6 = SQ_B6,
	WHITE_C6 = SQ_C6,
	WHITE_D6 = SQ_D6,
	WHITE_E6 = SQ_E6,
	WHITE_F6 = SQ_F6,
	WHITE_G6 = SQ_G6,
	WHITE_H6 = SQ_H6,
	BLACK_A3 = SQ_A3,
	BLACK_B3 = SQ_B3,
	BLACK_C3 = SQ_C3,
	BLACK_D3 = SQ_D3,
	BLACK_E3 = SQ_E3,
	BLACK_F3 = SQ_F3,
	BLACK_G3 = SQ_G3,
	BLACK_H3 = SQ_H3,
};

enum class Phase
{
	RUNNING,
	ENDED
};

// Macros for defining extra base operations on enumerations
// From Stockfish source code

#define ENABLE_BASE_OPERATORS_ON(T)                                \
constexpr T operator+(T d1, T d2) { return T(int(d1) + int(d2)); } \
constexpr T operator-(T d1, T d2) { return T(int(d1) - int(d2)); } \
constexpr T operator-(T d) { return T(-int(d)); }                  \
inline T& operator+=(T& d1, T d2) { return d1 = d1 + d2; }         \
inline T& operator-=(T& d1, T d2) { return d1 = d1 - d2; }

#define ENABLE_INCR_OPERATORS_ON(T)                                \
inline T& operator++(T& d) { return d = T(int(d) + 1); }           \
inline T& operator--(T& d) { return d = T(int(d) - 1); }

// End of external code

#ifdef _DEBUG
// Check if enum value is valid
#define ENABLE_VALIDITY_CHECK(T, MAX) \
inline constexpr bool T ## Check (T const& d) { return d >= 0 && d < MAX; }
#else
// Assume every value is valid
#define ENABLE_VALIDITY_CHECK(T, MAX) \
inline constexpr bool T ## Check (T const& d) { return true; }
#endif

#define ENABLE_MIRROR_OPERATOR_ON(T, MAX) \
inline T& operator~(T& d) { return d = T((int) MAX - 1 - (int) d); }

ENABLE_BASE_OPERATORS_ON(Direction)

ENABLE_INCR_OPERATORS_ON(Square)
ENABLE_INCR_OPERATORS_ON(File)
ENABLE_INCR_OPERATORS_ON(Rank)

ENABLE_VALIDITY_CHECK(Square, SQ_CNT);
ENABLE_VALIDITY_CHECK(File, FL_CNT)
ENABLE_VALIDITY_CHECK(Rank, RK_CNT)

ENABLE_MIRROR_OPERATOR_ON(Square, SQ_CNT)
ENABLE_MIRROR_OPERATOR_ON(File, FL_CNT)
ENABLE_MIRROR_OPERATOR_ON(Rank, RK_CNT)

#undef ENABLE_BASE_OPERATORS_ON
#undef ENABLE_INCR_OPERATORS_ON
#undef ENABLE_MIRROR_OPERATOR_ON

inline Rank getSquareRank(Square sq) { return Rank((int) sq >> 3); }
inline File getSquareFile(Square sq) { return File((int) sq & 0b111); }
inline Square getSquare(Rank r, File f) { return Square((int) r << 3 | (int) f); }

inline Direction operator-(Square dest, Square origin) {
	return Direction((int) dest - (int) origin);
}

inline Square& operator+=(Square& sq, Direction dir) {
	return sq = Square((int) sq + (int) dir);
}

inline Square operator+(Square sq, Direction dir) {
	return Square((int) sq + (int) dir);
}

inline Square& operator-=(Square& sq, Direction dir) {
	return sq = Square((int) sq - (int) dir);
}

inline Square operator-(Square sq, Direction dir) {
	return Square((int) sq - (int) dir);
}

inline Square getEnPassantPawnSquare(EnPassantPawn pawn) {
	return Square((int) pawn);
}

inline EnPassantPawn square2EnPassant(Square sq) {
	return EnPassantPawn((int) sq);
}