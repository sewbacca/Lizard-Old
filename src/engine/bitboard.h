#pragma once

#include "def.h"

namespace Bitboard
{
	bool is_set(bitboard, Square Square);
	bool is_single(bitboard n);
	bitboard cell(Square Square);
	Square index(bitboard board);
}
