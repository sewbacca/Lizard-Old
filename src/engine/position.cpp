
#include <cassert>
#include <algorithm>

#include "position.h"
#include "bitboard.h"
#include "hash.h"

Piece Position::get(int x, int y) {
	assert(is_inside(x, y));

	auto board = this->board();
	for(int p = 0; p < PIECE_TYPES; p++) {
		if(Bitboard::is_set(board[p], x, y))
			return (Piece) p;
	}

	return NO_PIECE;
}

void Position::set(int x, int y, Piece p) {
	assert(is_inside(x, y));
	
	bitboard* board = this->board();
	Piece onboard = get(x, y);

	// Delete piece
	if(onboard != NO_PIECE) {
		board[onboard] -= Bitboard::cell(x, y);

		// Find piece
		for(int i = 0; i < piececount[onboard]; i++) {
			Point* pos = &pieces[onboard][i];
			if(pos->x == x && pos->y == y) {
				std::swap(*pos, *(pieces[onboard] + piececount[onboard] - 1));
				piececount[onboard]--;
				break;
			}
		}
		
		m_hash ^= get_hash(x, y, onboard);
	}

	// Add piece
	if(p != NO_PIECE) {
		
		board[p] |= Bitboard::cell(x, y);
		pieces[p][piececount[p]] = Point(x, y);
		piececount[p]++;

		m_hash ^= get_hash(x, y, p);
	}
}

U64 Position::hash() {
	return m_hash ^
		get_hash(Bitboard::index(enpassantsq) % BOARD_SIZE) ^
		get_hash(side) ^
		get_hash(rights);
}