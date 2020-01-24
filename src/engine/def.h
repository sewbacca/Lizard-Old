
#pragma once

#include <cstdint>

typedef uint64_t bitboard;
typedef int_fast8_t byte;

constexpr int BOARD_SIZE = 8;
constexpr int PIECE_TYPES = 12;
enum Piece : byte
{
	WP, WN, WB, WR, WQ, WK,
	BP, BN, BB, BR, BQ, BK,
	NO_PIECE
};

bool is_inside(int x, int y);