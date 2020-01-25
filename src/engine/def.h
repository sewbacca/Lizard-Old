
#pragma once

#include <cstdint>
#include "misc/enum_operator.h"

#define startpos "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

typedef uint64_t bitboard;
typedef int_fast8_t byte;

constexpr int MAX_PIECES = 10;
constexpr int BOARD_SIZE = 8;
constexpr int PIECE_TYPES = 12;

enum Piece : byte
{
	WP, WN, WB, WR, WQ, WK,
	BP, BN, BB, BR, BQ, BK,
	NO_PIECE
};

enum Color
{
	WHITE,
	BLACK,
};

enum CastlingSide : byte
{
	KING_SIDE =	0b1010,
	QUEEN_SIDE =	0b0101,
	W_OO =		0b1000,
	W_OOO =		0b0100,
	B_OO =		0b0010,
	B_OOO =		0b0001,
	CS_WHITE =	0b1100,
	CS_BLACK =	0b0011,
	CS_ALL =	0b1111,
	NO_CASTLING =	0
};

struct Point {
	int x = 0, y = 0;
	Point(int x, int y) : x{x}, y{y} { };
	Point() { }
};

bool is_inside(int x, int y);

// Enum operator

enum_bitwise_operator_rhs(CastlingSide, byte, |)
enum_bitwise_operator_rhs(CastlingSide, byte, &)
enum_bitwise_operator_rhs(CastlingSide, byte, ^)
enum_bitwise_operator_invert(CastlingSide, byte, CastlingSide::CS_ALL)

enum_bitwise_operator_lhs(CastlingSide, |=, |)
enum_bitwise_operator_lhs(CastlingSide, &=, &)
enum_bitwise_operator_lhs(CastlingSide, ^=, ^)
