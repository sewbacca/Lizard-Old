
#include <cassert>
#include <iostream>
#include <algorithm>

#include "textutils.h"

#include "search.h"
#include "movegen.h"
#include "eval.h"
#include "attacks.h"
#include "pv.h"
#include "msclock.h"

static void check_up(SearchInfo& info)
{
	if (info.stopped) return;

	if (info.end <= msclock()) { info.stopped = true; }
}

static bool make_move(Move move, Position& pos)
{
	pos.makeMove(move);
	if (is_in_check(pos, swap(pos.side)))
	{
		pos.undoMove();
		return false;
	}

	return true;
}

static bool exists(Move move, Position& pos)
{
	if (move == NO_MOVE) return false;

	Move moves[MAX_MOVES];
	Move* end = gen_pseudo(pos, pos.side, moves);

	for (Move* cmp = moves; cmp < end; cmp++)
	{
		if (*cmp == move) { return true; }
	}

	return false;
}

static bool is_repetition(Position& pos)
{
	U64 hash { pos.hash() };

	for (int i { pos.hisply - 1 }; i >= (pos.hisply - pos.fiftyply); i--)
	{
		if (pos.history[i].hash == hash) { return true; }
	}

	return false;
}

static void next_move(Move* list, Move* end, Position& pos)
{
	// auto value = [](const Move& move, Position& pos) -> int {

	// };
	Move* maxmove = std::max_element(
	    list, end, [&pos](const Move& largest, const Move& cmp) { return largest.score < cmp.score; });
	std::swap(*list, *maxmove);

	assert(list->score >= maxmove->score);
}

static void index(Move* list, Move* end, Position& pos)
{
	const int VALUES[] = { 1, 2, 3, 4, 5, 6 };
	constexpr int pv { 500'000'000 };
	constexpr int capture { 400'000'000 };
	constexpr int killer0 = 300'000'000;
	constexpr int killer1 { 200'000'000 };
	U64 hash { pos.hash() };

	for (Move* move = list; move < end; move++)
	{
		if (probe_pv(hash) == *move) { move->score += pv; }
		if (move->capture() != NO_PIECE)
		{
			if (move->isEnPassant()) move->score += 1;
			move->score =
			    capture + VALUES[piece_type(move->capture())] * 100 - VALUES[piece_type(move->piece())];
		}
		if (pos.search_killers[pos.ply][0] == *move) move->score += killer0;
		if (pos.search_killers[pos.ply][1] == *move) move->score += killer1;
		move->score += pos.history_heuristic[move->from()][move->to()];
	}
}

static int alphabeta(int alpha, int beta, int depth, Position& pos, SearchInfo& info)
{
	if ((info.nodes & 2047) == 0 && !info.infinite) { check_up(info); }

	if (info.stopped) { return 0; }

	if (depth <= 0)
	{
		info.nodes++;
		;
		return evaluate(pos);	 // -quiesence(-beta, -alpha);
	}
	info.nodes++;

	if (pos.fiftyply >= 100 || is_repetition(pos)) { return 0; }
	else if (pos.ply >= MAX_DEPTH)
	{
		return evaluate(pos);
	}

	Move moves[MAX_MOVES];
	Move* end = gen_pseudo(pos, pos.side, moves);
	index(moves, end, pos);

	int legal_count { 0 };

	U64 hash { pos.hash() };

	for (Move* move = moves; move < end; move++)
	{
		next_move(move, end, pos);

		if (!make_move(*move, pos)) continue;
		legal_count++;	  // This is a legal move

		int node { -alphabeta(-beta, -alpha, depth - 1, pos, info) };
		pos.undoMove();
		if (info.stopped) return 0;

		if (node > alpha)
		{
			if (node >= beta)
			{
				if (legal_count == 1) info.fhf++;

				info.fh++;

				if (move->capture() == NO_PIECE)
				{
					pos.search_killers[pos.ply][1] = pos.search_killers[pos.ply][0];
					pos.search_killers[pos.ply][0] = *move;
				}
				return beta;
			}
			alpha = node;
			store_pv(hash, *move);

			if (move->capture() == NO_PIECE) { pos.history_heuristic[move->from()][move->to()] += depth; }
		}
	}

	if (legal_count == 0)
	{
		if (is_in_check(pos, pos.side)) { return -(MATE - pos.ply); }
		else
		{
			return 0;
		}
	}

	return alpha;
}

static void init_search(SearchInfo& info, Position& pos)
{
	clear_pv();

	info.start    = msclock();
	info.bestmove = NO_MOVE;
	info.nodes    = 0;

	pos.ply = 0;

	for (int x { 0 }; x < SQUARE_COUNT; x++)
	{
		for (int y { 0 }; y < SQUARE_COUNT; y++)
		{
			pos.history_heuristic[x][y] = 0;
		}
	}
	for (int ply { 0 }; ply < MAX_DEPTH; ply++)
	{
		pos.search_killers[ply][0] = NO_MOVE;
		pos.search_killers[ply][1] = NO_MOVE;
	}
}

void search(SearchInfo* info_ptr, Position* pos_ptr)
{
	/* Loop through moves
		-> Check move legality
		-> Make alpha beta search
		-> iterative deepening
	*/

	SearchInfo& info = *info_ptr;
	Position& pos	 = *pos_ptr;

	init_search(info, pos);

	Move moves[MAX_MOVES];
	Move* end = gen_pseudo(pos, pos.side, moves);
	index(moves, end, pos);
	std::sort(moves, end, [](const Move& before, const Move& after) { return before.score > after.score; });

	U64 hash { pos.hash() };

	for (size_t depth { 1 }; depth <= info.depth && !info.stopped; depth++)
	{
		Move* bestmove = moves;
		int cp { -INFINITE };

		for (Move* move = moves; move < end; move++)
		{
			if (!make_move(*move, pos)) continue;

			int score { -alphabeta(-INFINITE, -cp, depth - 1, pos, info) };
			if (score > cp)
			{
				cp	 = score;
				bestmove = move;

				store_pv(hash, *bestmove);
			}

			pos.undoMove();

			if (info.stopped) { goto plotmove; }
		}
		info.bestmove = *bestmove;

		U64 time_ms { msclock() - info.start };

		std::cout << "info depth " << depth << " nodes " << info.nodes << " time " << time_ms << " nodes "
			  << info.nodes << " score cp " << cp << " pv";

		size_t pv { 0 };
		for (size_t i { 0 }; i < depth; i++)
		{
			Move bestmove { probe_pv(pos.hash()) };
			if (!exists(bestmove, pos)) break;
			if (!make_move(bestmove, pos)) break;
			std::cout << ' ' << to_uci(bestmove);
			pv++;
		}

		for (size_t i { 0 }; i < pv; i++)
		{
			pos.undoMove();
		}

		std::cout << std::endl;
		// std::cout << "Ordering (" << info.fhf << " / " << info.fh << "): " << (float)info.fhf / info.fh << '\n';
	}

plotmove:
	std::cout << "bestmove " << to_uci(info.bestmove) << std::endl;
}