
#include <cmath>

#include "def.h"
#include "bitboard_constants.h"
#include "bitboard.h"

bitboard squares[SQUARE_COUNT][SQUARE_COUNT] {};

bitboard rectangle(Square start, Square end)
{
	return squares[start][end];
}

bitboard rank_bb(int rank)
{
	const static bitboard ranks[] = {
		RANK_1,
		RANK_2,
		RANK_3,
		RANK_4,
		RANK_5,
		RANK_6,
		RANK_7,
		RANK_8,
	};

	if(rank < 0 || rank > BOARD_SIZE)
		return 0ULL;

	return ranks[rank];
}

bitboard rank_bb_sqr(Square sq)
{
	return rank_bb(rank(sq));
}

bitboard file_bb(int file)
{
	const static bitboard files[] = {
		FILE_A,
		FILE_B,
		FILE_C,
		FILE_D,
		FILE_E,
		FILE_F,
		FILE_G,
		FILE_H,
	};


	if(file < 0 || file > BOARD_SIZE)
		return 0ULL;

	return files[file];
}

bitboard file_bb_sqr(Square sq)
{
	return file_bb(file(sq));
}


size_t init_bitboard_constants()
{
	auto calc_bb = [](int xstart, int ystart, int xend, int yend) {
		bitboard result {};
		int x { std::min(xstart, xend) };
		int y { std::min(ystart, yend) };
		xend = std::max(xend, xstart);
		yend = std::max(yend, ystart);
		xstart = x;
		ystart = y;
		
		while (is_inside(x, y))
		{
			result |= cell(idx(x, y));

			x++;
			if(x > xend)
			{
				x = xstart;
				y++;
			}

			if(y > yend)
				break;
		}

		return result;
	};

	size_t bytes = 0;
	for(Square start { 0 }; start < SQUARE_COUNT; start++)
	{
		for(Square end {0}; end < SQUARE_COUNT; end++)
		{
			squares[start][end] = calc_bb(file(start), rank(start), file(end), rank(end));
			bytes += sizeof(squares[start][end]);
		}
	}

	return bytes;
}