
#include "eval.h"

int evaluate(const Position& pos) {
	int score = 0;

	const int VALUES[] = {
		+100, +300, +300, +500, +900, 0,
		-100, -300, -300, -500, -900, 0
	};

	for(Piece p = WP; p < PIECE_TYPES; p++) {
		score += pos.piececount[p] * VALUES[p];
	}

	if(pos.side == BLACK) score *= -1;

	return score;
}