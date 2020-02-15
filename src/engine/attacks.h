
#pragma once

#include "def.h"
#include "position.h"

void init_attacks();

bitboard attacks_p(Square sq, Color col);
bitboard defenders_p(Square sq, Color col);
bitboard attacks_n(Square);
bitboard attacks_k(Square);
bitboard attacks_b(Square, bitboard occupied);
bitboard attacks_r(Square, bitboard occupied);
bitboard attacks_q(Square, bitboard occupied);