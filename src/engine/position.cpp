
#include <cassert>
#include <algorithm>

#include "position.h"
#include "bitboard.h"
#include "hash.h"

Piece Position::get(Square sq) {
	assert(is_inside(sq));

	auto board = this->board();
	for(int p = 0; p < PIECE_TYPES; p++) {
		if(Bitboard::is_set(board[p], sq))
			return (Piece) p;
	}

	return NO_PIECE;
}

void Position::set(Square sq, Piece p) {
	assert(is_inside(sq));
	
	bitboard* board = this->board();
	Piece onboard = get(sq);

	// Delete piece
	if(onboard != NO_PIECE) {
		board[onboard] -= Bitboard::cell(sq);

		// Find piece
		for(int i = 0; i < piececount[onboard]; i++) {
			Square* pos = &pieces[onboard][i];
			if(*pos == sq) {
				std::swap(*pos, *(pieces[onboard] + piececount[onboard] - 1));
				piececount[onboard]--;
				break;
			}
		}
		
		m_hash ^= get_hash(sq, onboard);
	}

	// Add piece
	if(p != NO_PIECE) {
		
		board[p] |= Bitboard::cell(sq);
		pieces[p][piececount[p]] = sq;
		piececount[p]++;

		m_hash ^= get_hash(sq, p);
	}
}

U64 Position::hash() {
	return m_hash ^
		get_hash(Bitboard::index(enpassantsq) % BOARD_SIZE) ^
		get_hash(side) ^
		get_hash(rights);
}