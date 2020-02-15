#include <cassert>

#include "hash.h"
#include "rand64.h"

U64 castling[16];
U64 black;
U64 enpassant[BOARD_SIZE];
U64 piece[PIECE_TYPES][SQUARE_COUNT];

size_t init_hash() {
	for(int i = 0; i < 16; i++) {
		castling[i] = rand64();
	}

	black = rand64();

	for(int i = 0; i < BOARD_SIZE; i++) {
		enpassant[i] = rand64();
	}

	for(Piece p = WP; p < PIECE_TYPES + 1; ++p) {
		for(int i = 0; i < SQUARE_COUNT; i++) {
			piece[p][i] = rand64();
		}
	}

	return sizeof(castling) +
		sizeof(black) +
		sizeof(enpassant) +
		sizeof(piece);
}

U64 get_hash(Color color) {
	assert(color == BLACK || color == WHITE);

	if(color == BLACK)
		return black;
	return 0;
}

U64 get_hash(Square sq, Piece p) {
	assert(is_inside(sq));

	if(p == NO_PIECE)
		return 0;

	return piece[p][sq];
}

U64 get_hash(CastlingSide rights) {
	assert(rights >= 0 && rights < 16);

	return castling[rights];
}

U64 get_hash(int enpassantx) {
	assert(enpassantx >= -1 && enpassantx < BOARD_SIZE);

	if(enpassantx == -1)
		return 0;

	return enpassant[enpassantx];
}
