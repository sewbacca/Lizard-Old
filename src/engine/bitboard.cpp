
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

bool Bitboard::is_single(bitboard n) {
	return n && (!(n & (n - 1))); 
}

int Bitboard::index(bitboard board) {
	if(!is_single(board))
		return -1;
	
	int index = 0;
	while(board != 0) {
		board -= 1ULL << index;
		index++;
	}

	return index;
}