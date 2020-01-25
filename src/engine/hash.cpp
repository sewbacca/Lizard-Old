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


U64 castling[32];
U64 black;
U64 enpassant[BOARD_SIZE];
U64 piece[PIECE_TYPES][BOARD_SIZE][BOARD_SIZE];

void init_hash() {
	for(int i = 0; i < 32; i++) {
		castling[i] = RAND64;
	}

	black = RAND64;

	for(int i = 0; i < BOARD_SIZE; i++) {
		enpassant[i] = RAND64;
	}

	for(int p = 0; p < PIECE_TYPES + 1; p++) {
		for(int x = 0; x < BOARD_SIZE; x++) {
			for(int y = 0; y < BOARD_SIZE; y++) {
				piece[p][x][y] = RAND64;
			}
		}
	}
}

U64 get_hash(Color color) {
	assert(color == BLACK || color == WHITE);

	if(color == BLACK)
		return black;
	return 0;
}

U64 get_hash(int x, int y, Piece p) {
	assert(is_inside(x, y));

	if(p == NO_PIECE)
		return 0;

	return piece[p][x][y];
}

U64 get_hash(CastlingSide rights) {
	return castling[rights];
}

U64 get_hash(int enpassantx) {
	assert(enpassantx >= -1 && enpassantx < BOARD_SIZE);

	if(enpassantx == -1)
		return 0;

	return enpassant[enpassantx];
}
