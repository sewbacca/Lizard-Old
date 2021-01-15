
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
		pos.make_move(*move);
		if (is_in_check(pos, side))
		{
			pos.undo_move();
			continue;
		}

		leaf_nodes += perft(depth - 1, pos);

		pos.undo_move();
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

	size_t total { 0 };

	std::cout << "Move count: " << end - moves << std::endl;

	for (Move* move = moves; move < end; move++)
	{
		pos.make_move(*move);
		if (is_in_check(pos, side))
		{
			pos.undo_move();
			continue;
		}

		size_t local_nodes { perft(depth - 1, pos) };
		total += local_nodes;

		std::cout << to_uci(*move) << ": " << local_nodes << std::endl;

		pos.undo_move();
	}

	std::cout << "Total: " << total << std::endl;
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
		pos.make_move(*move);
		if (is_in_check(pos, side))
		{
			pos.undo_move();
			continue;
		}

		step_through(depth - 1, pos);

		pos.undo_move();

		std::cout << to_string(pos) << std::endl;
		std::cin.get();
	}
}