
#include "eval.h"

// clang-format off

const int pawn_table[64] = {
	0,	0,	0,	0,	0,	0,	0,	0,
	10,	10,	0,	-10,	-10,	0,	10,	10,
	5,	0,	0,	5,	5,	0,	0,	5,
	0,	0,	10,	20,	20,	10,	0,	0,
	5,	5,	5,	10,	10,	5,	5,	5,
	10,	10,	10,	20,	20,	10,	10,	10,
	20,	20,	20,	30,	30,	20,	20,	20,
	0,	0,	0,	0,	0,	0,	0,	0,
};

const int knight_table[64] = {
	0,	-10,	0,	0,	0,	0,	-10,	0,
	0,	0,	0,	5,	5,	0,	0,	0,
	0,	0,	10,	10,	10,	10,	0,	0,
	0,	0,	10,	20,	20,	10,	5,	0,
	5,	10,	15,	20,	20,	15,	10,	5,
	5,	10,	10,	20,	20,	10,	10,	5,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
};

const int bishop_table[64] = {
	0,	0,	-10,	0,	0,	-10,	0,	0,
	0,	0,	0,	10,	10,	0,	0,	0,
	0,	0,	10,	15,	15,	10,	0,	0,
	0,	10,	15,	20,	20,	15,	10,	0,
	0,	10,	15,	20,	20,	15,	10,	0,
	0,	0,	10,	15,	15,	10,	0,	0,
	0,	0,	0,	10,	10,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0
};

const int rook_table[64] = {
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	0,	0,	5,	10,	10,	5,	0,	0,
	25,	25,	25,	25,	25,	25,	25,	25,
	0,	0,	5,	10,	10,	5,	0,	0,
};

const int mirror_table[64] = {
	56,	57,	58,	59,	60,	61,	62,	63,
	48,	49,	50,	51,	52,	53,	54,	55,
	40,	41,	42,	43,	44,	45,	46,	47,
	32,	33,	34,	35,	36,	37,	38,	39,
	24,	25,	26,	27,	28,	29,	30,	31,
	16,	17,	18,	19,	20,	21,	22,	23,
	8,	9,	10,	11,	12,	13,	14,	15,
	0,	1,	2,	3,	4,	5,	6,	7,
};

// clang-format on

#define MIRROR64(sq) (mirror_table[(sq)])

int evaluate(const Position& pos)
{
	int score { 0 };

	const int VALUES[] = { +100, +300, +300, +500, +900, 0, -100, -300, -300, -500, -900, 0 };

	for (Piece p { WP }; p < PIECE_COUNT; p++)
	{
		score += pos.piececount[p] * VALUES[p];
	}

	const int* LISTS[] = { pawn_table, bishop_table, knight_table, rook_table };

	for (Piece p { WP }; p < PIECE_COUNT; p++)
	{
		PieceType pt { piece_type(p) };
		if (pt >= PAWN && pt <= ROOK)
		{
			const int* table = LISTS[pt];

			Piece WhitePiece { combine(WHITE, pt) };
			for (int i { 0 }; i < pos.piececount[WhitePiece]; i++)
			{
				score += table[pos.piecepos[WhitePiece][i]];
			}
			Piece BlackPiece { combine(BLACK, pt) };
			for (int i { 0 }; i < pos.piececount[BlackPiece]; i++)
			{
				score -= table[MIRROR64(pos.piecepos[BlackPiece][i])];
			}
		}
	}

	if (pos.side == BLACK) score *= -1;

	return score;
}