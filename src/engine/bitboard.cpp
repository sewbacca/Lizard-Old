
#include <cassert>

#include "bitboard.h"

bitboard Bitboard::cell(int x, int y) {
	assert(is_inside(x, y));

	return 1ULL << (y * BOARD_SIZE + x);
}

bool Bitboard::is_set(bitboard board, int x, int y) {
	assert(is_inside(x, y));

	return board & cell(x, y);
}