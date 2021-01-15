
#pragma once
#include "def.h"

enum Horizontal : int
{
	OFF_WEST = -1,
	OFF_NONE = 0,
	OFF_EAST = 1
};

enum Vertical : int
{
	OFF_NORTH = 1,
	OFF_SOUTH = -1
};

size_t init_bitboard_constants();
bitboard file_bb(int file);
bitboard file_bb_sqr(Square sq);
bitboard rank_bb(int rank);
bitboard rank_bb_sqr(Square sq);
bitboard rectangle(Square start, Square end);

constexpr bitboard shift(bitboard b, Horizontal offX, Vertical offY);

constexpr bitboard RANK_1 { 0xFF };
constexpr bitboard RANK_2 { RANK_1 << BOARD_SIZE };
constexpr bitboard RANK_3 { RANK_2 << BOARD_SIZE };
constexpr bitboard RANK_4 { RANK_3 << BOARD_SIZE };
constexpr bitboard RANK_5 { RANK_4 << BOARD_SIZE };
constexpr bitboard RANK_6 { RANK_5 << BOARD_SIZE };
constexpr bitboard RANK_7 { RANK_6 << BOARD_SIZE };
constexpr bitboard RANK_8 { RANK_7 << BOARD_SIZE };

constexpr bitboard FILE_A { 0x01'01'01'01'01'01'01'01 };
constexpr bitboard FILE_B { FILE_A << 1 };
constexpr bitboard FILE_C { FILE_B << 1 };
constexpr bitboard FILE_D { FILE_C << 1 };
constexpr bitboard FILE_E { FILE_D << 1 };
constexpr bitboard FILE_F { FILE_E << 1 };
constexpr bitboard FILE_G { FILE_F << 1 };
constexpr bitboard FILE_H { FILE_G << 1 };

constexpr bitboard shift(bitboard b, Horizontal offX, Vertical offY)
{
	int move { idx(offX, offY) };
	if (move >= 0)
	{
		b <<= move;
	}
	else
	{
		b >>= -move;
	}

	if (offX == OFF_WEST)
	{
		b &= ~FILE_H;
	}
	else if (offX == OFF_EAST)
	{
		b &= ~FILE_A;
	}

	return b;
}