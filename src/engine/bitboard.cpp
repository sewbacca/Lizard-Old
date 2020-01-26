
#include <cassert>

#include "bitboard.h"

bitboard Bitboard::cell(Square Square) {
	assert(is_inside(Square));

	return 1ULL << Square;
}

bool Bitboard::is_set(bitboard board, Square Square) {
	assert(is_inside(Square));

	return board & cell(Square);
}

bool Bitboard::is_single(bitboard n) {
	return n && (!(n & (n - 1))); 
}

Square Bitboard::index(bitboard board) {
	if(!is_single(board))
		return -1;
	
	Square index = 0;
	while(board != 0) {
		board -= 1ULL << index;
		index++;
	}

	return index;
}