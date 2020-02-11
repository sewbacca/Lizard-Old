
#pragma once

#include "def.h"

bool is_single(bitboard n);
bool is_set(bitboard, Square Square);
bitboard cell(Square Square);
// Returns the least significant bit
Square lsb(bitboard board);
// Returns the most significant bit
Square msb(bitboard board);
