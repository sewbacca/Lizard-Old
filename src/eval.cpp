
#include "gamephase.h"
#include "eval.h"
#include "movegen.h"
#include "attacks.h"
#include "bitboard.h"
#include "bitboard_constants.h"
#include "textutils.h"
#include <iostream>
#include <functional>

const int VALUES[] = { +100, +300, +300, +500, +900, 0, -100, -300, -300, -500, -900, 0 };

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
	int no_pawns { -30 };
	int aim_piece { -10 };
	int castling { 50 };
}

namespace GoodSquares
{
	const int open_file { 5 };
	const int seventh_rank { 5 };
	const int mobility[] { 0, 6, 5, 3, 3, 0 };

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

int pawn_structure(bitboard pawns, bitboard enemy_pawns, int ground_rank)
{
	int score = { 0 };
	int enemy_rank { BOARD_SIZE - ground_rank - 1};
	int walk = { ground_rank == 0 ? 1 : -1 };

	// bitboard pawn_atk { shift(pawns, Horizontal::OFF_WEST, Vertical { walk }) | shift(pawns, Horizontal::OFF_EAST, Vertical { walk }) };

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
	GamePhase phase { phase_of(pos) };

	// Give score for pieces.

	for (Piece p { WP }; p < PIECE_COUNT; p++)
	{
		score += pos.piececount[p] * VALUES[p];
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

	bool black_castling = (pos.rights || pos.done_castles) & CS_BLACK;
	bool white_castling = (pos.rights || pos.done_castles) & CS_WHITE;

	if(white_castling)
		score += KingSafety::castling;

	if(black_castling)
		score -= KingSafety::castling;

	if(false && phase != GamePhase::END_GAME)
	{
		auto count_pieces = [](Square king_pos, Vertical direction, bitboard test_pieces) {
			auto horizontal_surroundings = [](Square pos) {
				int x { file(pos) };
				int y { rank(pos) };

				return rectangle(idx_capped(x - 1, y), idx_capped(x + 1, y));
			};

			bitboard pieces { horizontal_surroundings(king_pos) };
			pieces = shift(pieces, Horizontal::OFF_NONE, direction);
			pieces &= test_pieces;

			return pop_count(pieces);
		};

		Square wk_pos { pos.piecepos[WK][0] };
		Square bk_pos { pos.piecepos[BK][0] };


		if(count_pieces(wk_pos, Vertical::OFF_NORTH, pos.pieces(WP)) == 0)
			score += KingSafety::no_pawns;
		
		if(count_pieces(bk_pos, Vertical::OFF_SOUTH, pos.pieces(BP)) == 0)
			score -= KingSafety::no_pawns;

		// static const PieceType AIM_PIECES[] { ROOK, BISHOP };

		int bk_attacker = 0;
		bk_attacker += pop_count(pos.pieces(WR) & attacks_r(bk_pos, 0ULL));
		bk_attacker += pop_count(pos.pieces(WB) & attacks_b(bk_pos, 0ULL));
		int wk_attacker = 0;
		wk_attacker += pop_count(pos.pieces(BR) & attacks_r(wk_pos, 0ULL));
		wk_attacker += pop_count(pos.pieces(BB) & attacks_b(wk_pos, 0ULL));

		score += bk_attacker * KingSafety::aim_piece;
		score -= wk_attacker * KingSafety::aim_piece;
	}

	// Swap score if it is blacks turn.

	if (pos.side == BLACK) score *= -1;

	return score;
}