
#pragma once
#include "def.h"

bitboard file_bb(Square sq);
bitboard rank_bb(Square sq);

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