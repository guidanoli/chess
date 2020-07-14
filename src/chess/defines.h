#pragma once

namespace chesslib
{
	constexpr auto major_version = 1;
	constexpr auto minor_version = 1;
}

#if defined(_MSC_VER)
#pragma warning(disable: 26812) // Unescoped enum type
#endif

// A `op` B = C
#define ENABLE_BINARY_OPERATION_ON(A, op, B, C) \
inline constexpr auto operator op(A a, B b) { \
	return static_cast<C>(static_cast<int>(a) op static_cast<int>(b)); }

// A `op` B = C ^ B `op` A = C
#define ENABLE_COMUTATIVE_BINARY_OPERATION_ON(A, op, B, C) \
ENABLE_BINARY_OPERATION_ON(A, op, B, C) \
ENABLE_BINARY_OPERATION_ON(B, op, A, C)

// -A
#define ENABLE_NEGATION_OPERATION_ON(T) \
inline constexpr auto operator-(T d) { \
	return static_cast<T>(-static_cast<int>(d)); }

// A `op`= B
#define ENABLE_ACTION_AND_ASSIGNMENT_OPERATION_ON(A, op, B) \
inline constexpr auto& operator  op ## =(A& a, B b) { \
	return a = a op b; }

// ACheck (0 <= A <= MAX)
#define ENABLE_VALIDITY_CHECK(A, MAX) \
inline constexpr bool A ## Check (A const& d) { \
	return static_cast<int>(d) >= 0 && static_cast<int>(d) < static_cast<int>(MAX); }

// ~A (MAX - 1 - A)
#define ENABLE_MIRROR_OPERATOR_ON(T, MAX) \
inline constexpr auto operator~(T d) { \
	return static_cast<T>(static_cast<int>(MAX) - 1 - static_cast<int>(d)); }

// A++ ^ A--
#define ENABLE_INCR_OPERATORS_ON(T)	\
inline constexpr auto& operator++(T& d) { \
	return d = static_cast<T>(static_cast<int>(d) + 1); } \
inline constexpr auto& operator--(T& d) { \
	return d = static_cast<T>(static_cast<int>(d) - 1); }

// T > T ^ T < T ^ T >= T ^ T <= T
#define ENABLE_COMPARE_OPERATOR_ON(T) \
ENABLE_BINARY_OPERATION_ON(T, >, T, bool) \
ENABLE_BINARY_OPERATION_ON(T, <, T, bool) \
ENABLE_BINARY_OPERATION_ON(T, >=, T, bool) \
ENABLE_BINARY_OPERATION_ON(T, <=, T, bool)
