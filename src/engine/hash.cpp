#include <cassert>

#include "hash.h"
#include <cstdlib>

#define RAND64 (\
            static_cast<U64>(std::rand()) +\
            (static_cast<U64>(std::rand()) << 15) +\
            (static_cast<U64>(std::rand()) << 30) +\
            (static_cast<U64>(std::rand()) << 45) +\
            (static_cast<U64>(std::rand() & 0xf) << 60)\
        )


U64 castling[16];
U64 black;
U64 enpassant[BOARD_SIZE];
U64 piece[PIECE_TYPES][SQUARE_COUNT];

void init_hash() {
	for(int i = 0; i < 16; i++) {
		castling[i] = RAND64;
	}

	black = RAND64;

	for(int i = 0; i < BOARD_SIZE; i++) {
		enpassant[i] = RAND64;
	}

	for(int p = 0; p < PIECE_TYPES + 1; p++) {
		for(int i = 0; i < SQUARE_COUNT; i++) {
			piece[p][i] = RAND64;
		}
	}
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
