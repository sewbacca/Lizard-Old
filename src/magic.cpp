
// Highly inspired by Stockfish: https://github.com/official-stockfish/Stockfish/blob/master/src/bitboard.cpp

#include <array>
#include <cassert>
#include "magic.h"
#include "bitboard.h"
#include "bitboard_constants.h"
#include "rand64.h"

namespace Magic
{
bitboard ROOK_ATKS[0xfffff];
bitboard BISHOP_ATKS[0xfffff];
MagicBitboard MAGIC_ROOKS[SQUARE_COUNT];
MagicBitboard MAGIC_BISHOPS[SQUARE_COUNT];
}    // namespace Magic

struct Vector2D
{
	int x { 0 }, y { 0 };
	constexpr Vector2D(int x, int y) : x { x }, y { y } {}
};

constexpr Vector2D operator+(Vector2D a, Vector2D b) { return Vector2D { a.x + b.x, a.y + b.y }; }

constexpr Vector2D NORTH { 0, 1 }, EAST { 1, 0 }, SOUTH { 0, -1 }, WEST { -1, 0 };
constexpr Vector2D NE { NORTH + EAST }, SE { SOUTH + EAST }, NW { NORTH + WEST }, SW { SOUTH + WEST };

static bitboard sliding(Square start_sq, bitboard occupied, std::array<Vector2D, 4> deltas)
{
	bitboard attacks { 0 };
	Vector2D start { Vector2D(file(start_sq), rank(start_sq)) };
	for (Vector2D delta : deltas)
	{
		for (Vector2D sq { start + delta }; is_inside(sq.x, sq.y); sq = sq + delta)
		{
			bitboard atk { cell(idx(sq.x, sq.y)) };
			attacks |= atk;
			if (occupied & atk) break;
		}
	}

	assert(attacks);

	return attacks;
}

// Fancy Magic Bitboards: https://www.chessprogramming.org/Magic_Bitboards#Fancy
static void init_cache(std::array<Vector2D, 4> deltas, Magic::MagicBitboard magic_list[], bitboard table[])
{
	constexpr bitboard FILE_EDGES { FILE_A | FILE_H };
	constexpr bitboard RANK_EDGES { RANK_1 | RANK_8 };

	// Init magic

	// magic_index -> occupied, magic_index -> attacks
	bitboard occupied[4096], reference_atks[4096];

	// some magic_index -> attempt count to find a magic
	int epoch[4096] = {};
	int size { 0 };
	int cnt { 0 };

	for (Square sq { 0 }; is_inside(sq); sq++)
	{
		// We don't need the edges
		bitboard edges { (RANK_EDGES & ~rank_bb(sq)) | (FILE_EDGES & ~file_bb(sq)) };

		Magic::MagicBitboard& w = magic_list[sq];

		// Mask are sliding attacks on empty board - irrelevant bits for computing the index to the attacks
		w.mask	= sliding(sq, 0ULL, deltas) & ~edges;
		w.shift = 64 - pop_count(w.mask);
		// The start of the attack store
		w.list = sq == 0 ? table : magic_list[sq - 1].list + size;

		// Iterate through all occupied combinations with the Carry-Rippler trick (https://www.chessprogramming.org/Traversing_Subsets_of_a_Set#All_Subsets_of_any_Set)
		// Basicly we create the data used later inside movegen
		bitboard occ { 0 };
		size = 0;
		do
		{
			occupied[size]	     = occ;
			reference_atks[size] = sliding(sq, occ, deltas);
			// A magic trick i don't understand
			// I think you kinda subtract 1 from a virtual number, masked by mask
			occ = (occ - w.mask) & w.mask;

			size++;
		} while (occ);

		// Now lets search for magic spells (bitboards)

		for (int i { 0 }; i < size;)
		{
			w.magic = rand64_fewbits();

			cnt++;
			for (i = 0; i < size; i++)
			{
				int magic_index { w.index(occupied[i]) };
				if (epoch[magic_index] < cnt)
				{    // Didn't tried yet?
					w.list[magic_index] = reference_atks[i];
					epoch[magic_index]  = cnt;
				}
				else if (reference_atks[i] != w.list[magic_index])
				{
					break;
				}
			}
		}
	}
}

size_t Magic::init_magic()
{
	constexpr std::array<Vector2D, 4> D_ROOKS { NORTH, EAST, SOUTH, WEST };
	constexpr std::array<Vector2D, 4> D_BISHOPS { NE, SE, SW, NW };

	init_cache(D_ROOKS, MAGIC_ROOKS, ROOK_ATKS);
	init_cache(D_BISHOPS, MAGIC_BISHOPS, BISHOP_ATKS);

	return sizeof(ROOK_ATKS) + sizeof(BISHOP_ATKS) + sizeof(MAGIC_ROOKS) + sizeof(MAGIC_BISHOPS);
}