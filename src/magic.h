
#pragma once

#include "def.h"

namespace Magic
{
struct MagicBitboard
{
	bitboard* list;
	bitboard mask;
	bitboard magic;
	int shift;

	// Magic indexing

	unsigned index(bitboard occupied)
	{
		occupied &= mask;
		occupied *= magic;
		occupied >>= shift;
		return occupied;
	}
};

extern MagicBitboard MAGIC_ROOKS[SQUARE_COUNT];
extern MagicBitboard MAGIC_BISHOPS[SQUARE_COUNT];

size_t init_magic();
}    // namespace Magic