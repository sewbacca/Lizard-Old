
#pragma once

#include "def.h"

void init_hash();

U64 get_hash(Color);
U64 get_hash(int x, int y, Piece);
U64 get_hash(CastlingSide);
U64 get_hash(int enpassantx);