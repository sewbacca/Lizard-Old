
#include <cassert>

#include "bitboard.h"

bitboard cell(Square Square) {
	if(!is_inside(Square)) return 0;

	return 1ULL << Square;
}

bool is_set(bitboard board, Square Square) {
	assert(is_inside(Square));

	return board & cell(Square);
}

bool is_single(bitboard n) {
	return n && (!(n & (n - 1))); 
}

// Returns the least significant bit
Square lsb(bitboard board) {
	if(!board)	return -1;

	return __builtin_ctzll(board);
}

// Returns the most significant bit
Square msb(bitboard board) {
	if(!board)	return -1;

	return 63 ^ __builtin_clzll(board);
}

Square pop_lsb(bitboard& board) {
	Square sq = lsb(board);
	board &= board - 1;

	return sq;
}

int pop_count(bitboard board) {
	return __builtin_popcountll(board);
}