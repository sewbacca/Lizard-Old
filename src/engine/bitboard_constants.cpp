#include "bitboard_constants.h"

bitboard rank_bb(Square sq) {
	int r = rank(sq);
	const bitboard ranks[] = {
		RANK_1,
		RANK_2,
		RANK_3,
		RANK_4,
		RANK_5,
		RANK_6,
		RANK_7,
		RANK_8,
	};

	return ranks[r];
}

bitboard file_bb(Square sq) {
	int f = file(sq);
	const bitboard files[] = {
		FILE_A,
		FILE_B,
		FILE_C,
		FILE_D,
		FILE_E,
		FILE_F,
		FILE_G,
		FILE_H,
	};

	return files[f];
}