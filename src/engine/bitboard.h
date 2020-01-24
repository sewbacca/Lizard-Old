#pragma once

#include "def.h"

namespace Bitboard
{
	bool is_set(bitboard, int x, int y);
	bool is_single(bitboard n);
	bitboard cell(int x, int y);
	int index(bitboard board);
}
