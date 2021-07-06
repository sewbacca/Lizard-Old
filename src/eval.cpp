
#include "gamephase.h"
#include "eval.h"
#include "movegen.h"
#include "attacks.h"
#include "bitboard.h"
#include "bitboard_constants.h"
#include "textutils.h"
#include <iostream>
#include <functional>

const int VALUES[] = { +1, +3, +3, +5, +9, 0, -1, -3, -3, -5, -9, 0 };

namespace PawnStructures
{
	// Negative
	int isolani { -10 };
	int backpawn { -5 };
	int blocked_pawn { -5 };
	int double_pawns { -30 };

	// Positive
	int running_pawn { 35 };
	int free_pawn { 25 };
}

namespace SmallPiece
{
	int bishop_pair { 15 };
}

namespace KingSafety
{
	int move_penalty { -5 };
	int move_threshold { 3 };
}

namespace GoodSquares
{
	const int open_file { 5 };
	const int seventh_rank { 5 };
	const int mobility[] { 0, 6, 5, 3, 3, 0 };
	const int defender_penalty { -15 };

	const int knight[64] = {
		-10,	-5,	-5,	-5,	-5,	-5,	-5,	-10,
		-5,	0,	0,	5,	5,	0,	0,	-5,
		-5,	0,	10,	10,	10,	10,	0,	-5,
		-5,	0,	10,	20,	20,	10,	5,	-5,
		0,	10,	15,	20,	20,	15,	10,	0,
		0,	10,	10,	20,	20,	10,	10,	0,
		-5,	0,	5,	10,	10,	5,	0,	-5,
		-10,	-5,	-5,	-5,	-5,	-5,	-5,	-10,
	};

	const int bishop[64] = {
		0,	0,	-10,	0,	0,	-10,	0,	0,
		0,	0,	0,	10,	10,	0,	0,	0,
		0,	0,	10,	15,	15,	10,	0,	0,
		0,	10,	15,	20,	20,	15,	10,	0,
		0,	10,	15,	20,	20,	15,	10,	0,
		0,	0,	10,	15,	15,	10,	0,	0,
		0,	0,	0,	10,	10,	0,	0,	0,
		0,	0,	0,	0,	0,	0,	0,	0
	};

	const int mirror[64] = {
		56,	57,	58,	59,	60,	61,	62,	63,
		48,	49,	50,	51,	52,	53,	54,	55,
		40,	41,	42,	43,	44,	45,	46,	47,
		32,	33,	34,	35,	36,	37,	38,	39,
		24,	25,	26,	27,	28,	29,	30,	31,
		16,	17,	18,	19,	20,	21,	22,	23,
		8,	9,	10,	11,	12,	13,	14,	15,
		0,	1,	2,	3,	4,	5,	6,	7,
	};

	#define MIRROR64(sq) (GoodSquares::mirror[(sq)])
}

namespace Endgame
{
	const int maxmaterial { 2 * (9 * 1 + 4 * 3 + 2 * 5 + 9) };
	constexpr double ratio (double totalmaterial)
	{
		int x { clamp(1 - totalmaterial / Endgame::maxmaterial, 0, 1) };
		return x * x * x;
	}
}

static int pawn_structure(bitboard pawns, bitboard enemy_pawns, int ground_rank)
{
	int score = { 0 };
	int enemy_rank { BOARD_SIZE - ground_rank - 1};
	int walk = { ground_rank == 0 ? 1 : -1 };

	// Double pawns
	for(int x { 0 }; x < BOARD_SIZE; x++)
	{
		bitboard file { file_bb(x)};

		score += (pop_count(file & pawns) - 1) * PawnStructures::double_pawns;
	}

	bitboard remaining { pawns };
	while(remaining)
	{
		Square square { pop_lsb(remaining) };
		bitboard square_bb { cell(square) };

		int x { file(square) };
		int y { rank(square) };

		bitboard file { file_bb(x) };
		bitboard support_pawns = { rectangle(idx_capped(x - 1, y), idx_capped(x + 1, ground_rank)) };
		support_pawns &= ~file & pawns;
		bitboard blocking_pawns = { rectangle(idx_capped(x - 1, y + walk), idx_capped(x + 1, enemy_rank)) };
		blocking_pawns &= enemy_pawns;

		if(pop_count(support_pawns) == 0)
		{
			score +=  PawnStructures::isolani;

			if(!(rectangle(square, idx_capped(x, enemy_rank)) & enemy_pawns & ~square_bb))
				score += PawnStructures::backpawn;
		}

		if(!(cell(x, y + walk) & enemy_pawns))
			score += PawnStructures::blocked_pawn;

		if(pop_count(blocking_pawns) == 0)
		{
			score += PawnStructures::free_pawn;	

			if(std::abs(y - enemy_rank) <= 3)
				score += PawnStructures::running_pawn;
		}
	}

	return score;
}

int evaluate(Position& pos)
{
	int score { 0 };

	int white_mat { };
	int black_mat { };
	double endgame_ratio = Endgame::ratio(black_mat + white_mat);

	// Give score for pieces.

	for (Piece p { WP }; p < PIECE_COUNT; p++)
	{
		score += pos.piececount[p] * VALUES[p] * 100;

		if(piece_col(p) == WHITE)	
			white_mat += pos.piececount[p] * VALUES[piece_type(p)];
		else
			black_mat += pos.piececount[p] * VALUES[piece_type(p)];
	}

	// Give score for generally good squares


	const int* LISTS[] = { nullptr, GoodSquares::bishop, GoodSquares::knight };

	for (Piece p { WP }; p < PIECE_COUNT; p++)
	{
		PieceType pt { piece_type(p) };
		if (pt > PAWN && pt < ROOK)
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

	score += pawn_structure(pos.pieces(WP), pos.pieces(BP), 0);
	score -= pawn_structure(pos.pieces(BP), pos.pieces(WP), 7);

	score += pos.piececount[WB] >= 2 ? SmallPiece::bishop_pair : 0;
	score -= pos.piececount[BB] >= 2 ? SmallPiece::bishop_pair : 0;

	// Rook positioning

	int open_file_count = 0;
	int b_on_open = 0;
	int w_on_open = 0;

	for(int x = 0; x < BOARD_SIZE; x++)
	{
		bitboard file { file_bb(x) };
		bool open { !(file & (pos.pieces(WP) | pos.pieces(BP))) };
		
		if(open)
		{
			open_file_count++;


			w_on_open += pop_count(file & pos.pieces(WR));
			b_on_open += pop_count(file & pos.pieces(BR));
		}
	}

	if(open_file_count > 0 && open_file_count < 3)
	{
		score += GoodSquares::open_file * w_on_open;
		score -= GoodSquares::open_file * b_on_open;
	}

	int on_seventh { pop_count(pos.pieces(WR) & RANK_7) };
	if(pos.piececount[BP] >= 4 && on_seventh)
		score += on_seventh * GoodSquares::seventh_rank;

	int on_second { pop_count(pos.pieces(BR) & RANK_2) };
	if(pos.piececount[WP] >= 4 && on_second)
		score -= on_second * GoodSquares::seventh_rank;

	// Mobility

	const static std::function<bitboard(Square, bitboard)> attack[] { nullptr, [](Square sq, bitboard occ) { return attacks_n(sq); }, attacks_b, attacks_r, attacks_q, nullptr};

	bitboard attacks[12] {};
	bitboard attacked_by[2] {};

	for(Piece p = WP; p < NO_PIECE; p++)
	{
		PieceType pt { piece_type(p) };
		Color col { piece_col(p) };
		int count = 0;

		if(pt == PAWN || pt == KING)
			continue;
		
		
		for(int i = 0; i < pos.piececount[p]; i++)
		{
			Square square = pos.piecepos[p][i];
			attacks[p] |= attack[pt](square, pos.pieces());
			attacked_by[col] |= attacks[p];
		}

	}

	for(Piece p = WP; p < NO_PIECE; p++)
	{
		PieceType pt { piece_type(p) };
		Color col { piece_col(p) };
		
		if(pt == PAWN || pt == KING)
			continue;
		
		int count = pop_count(attacks[p] & ~attacked_by[swap(col)]);

		score += (col == WHITE ? 1 : -1) * count * GoodSquares::mobility[pt];
	}

	// Kingsafety

	if(endgame_ratio < 0.8f)
	{
		auto king_mobility = [&pos](Square king, bitboard pieces) {
			return pop_count(attacks_q(king, pieces) & ~pieces);
		};

		int wk_attackness { king_mobility(pos.piecepos[WK][0], pos.pieces(WP) & pos.pieces(WR)) };
		int bk_attackness { king_mobility(pos.piecepos[BK][0], pos.pieces(BP) & pos.pieces(BR)) };

		if(wk_attackness > KingSafety::move_threshold)
			score += KingSafety::move_penalty * wk_attackness * black_mat;

		if(bk_attackness > KingSafety::move_threshold)
			score -= KingSafety::move_penalty * bk_attackness * white_mat;
	}

	// Swap score if it is blacks turn.

	if (pos.side == BLACK) score *= -1;

	return score;
}