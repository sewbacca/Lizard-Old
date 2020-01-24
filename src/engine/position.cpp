
#include <cassert>

#include "position.h"
#include "bitboard.h"

Piece Position::get(int x, int y) {
	assert(is_inside(x, y));

	auto board = this->board();
	for(int p = 0; p < PIECE_TYPES; p++) {
		if(!Bitboard::is_set(board[p], x, y))
			continue;
		
		return (Piece) p;
	}

	return NO_PIECE;
}

void Position::set(int x, int y, Piece p) {
	assert(is_inside(x, y));

	auto board = this->board();
	bitboard cell = Bitboard::cell(x, y);
	if(p == NO_PIECE) {
		for(int p = 0; p < PIECE_TYPES; p++) {
			board[p] &= ~cell;
		}
	}
	else
		board[p] |= cell;
}