
#include "perft.h"

#include <iostream>

#include "movegen.h"
#include "attacks.h"
#include "textutils.h"

size_t perft(size_t depth, Position& pos)
{
	if (depth == 0) return 1;

	Color side { pos.side };

	Move moves[MAX_MOVES];
	Move* end = gen_pseudo(pos, side, moves);

	size_t leaf_nodes { 0 };

	for (Move* move = moves; move < end; move++)
	{
		pos.makeMove(*move);
		if (is_in_check(pos, side))
		{
			pos.undoMove();
			continue;
		}

		leaf_nodes += perft(depth - 1, pos);

		pos.undoMove();
	}

	return leaf_nodes;
}

void perft_divide(size_t depth, Position& pos)
{
	if (depth == 0)
	{
		std::cout << "Total: 0" << std::endl;
		return;
	}

	Color side { pos.side };

	Move moves[MAX_MOVES];
	Move* end = gen_pseudo(pos, side, moves);

	size_t leaf_nodes { 0 };

	std::cout << "Move count: " << end - moves << std::endl;

	for (Move* move = moves; move < end; move++)
	{
		pos.makeMove(*move);
		if (is_in_check(pos, side))
		{
			pos.undoMove();
			continue;
		}

		size_t local_nodes { perft(depth - 1, pos) };
		leaf_nodes += local_nodes;

		std::cout << to_uci(*move) << ": " << local_nodes << std::endl;

		pos.undoMove();
	}

	std::cout << "Total: " << leaf_nodes << std::endl;
}

void step_through(size_t depth, Position& pos)
{
	std::cout << to_string(pos) << std::endl;
	std::cin.get();
	if (depth == 0)
	{
		std::cout << "Total: 0" << std::endl;
		return;
	}

	Color side { pos.side };

	Move moves[MAX_MOVES];
	Move* end = gen_pseudo(pos, side, moves);

	for (Move* move = moves; move < end; move++)
	{
		pos.makeMove(*move);
		if (is_in_check(pos, side))
		{
			pos.undoMove();
			continue;
		}

		step_through(depth - 1, pos);

		pos.undoMove();

		std::cout << to_string(pos) << std::endl;
		std::cin.get();
	}
}