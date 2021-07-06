
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
	if (info.cur_depth <= 1)
		return;

	U64 cur_time { msclock() };
	if (info.end <= cur_time)
		info.stopped = true;
	else if (info.progress > 0.3 && info.end_estimate > info.end)
		info.stopped = true;
}

static bool exists(Move move, const Position& pos)
{
	Move  moves[MAX_MOVES];
	Move* end = gen_pseudo(pos, pos.side, moves);

	return std::find(moves, end, move) != end;
}

static bool is_repetition(Position& pos)
{
	U64 hash { pos.hash() };

	for (int i { pos.hisply - 1 }; i >= (pos.hisply - pos.fiftyply); i--)
	{
		if (pos.history[i].hash == hash)
			return true;
	}

	return false;
}

static void next_move(Move* list, Move* end)
{
	Move* maxmove = std::max_element(
		list, end,
		[](const Move& largest, const Move& cmp) {
			return largest.score < cmp.score;
		}
	);
	std::swap(*list, *maxmove);

	assert(list->score >= maxmove->score);
}

static void index(Move* list, Move* end, Position& pos)
{
	const int     VALUES[] = { 1, 2, 3, 4, 5, 6 };
	constexpr int pv         { 1'000'000'000 };
	constexpr int capture    {   400'000'000 };
	constexpr int killer0    {   300'000'000 };
	constexpr int killer1    {   200'000'000 };
	U64           hash       { pos.hash() };
	Move          pv_move    { probe_pv(hash) };

	for (Move* p_move = list; p_move < end; p_move++)
	{
		Move& move { *p_move };

		if (pv_move == move)
		{
			move.score += pv;
		}
		if (move.capture() != NO_PIECE)
		{
			if (move.isEnPassant())
				move.score += 1;

			move.score +=
				capture + VALUES[piece_type(move.capture())] * 100 - VALUES[piece_type(move.piece())];
		}
		if (pos.search_killers[pos.ply][0] == move)
			move.score += killer0;
		if (pos.search_killers[pos.ply][1] == move)
			move.score += killer1;
		move.score += pos.history_heuristic[move.from()][move.to()];
	}
}

static int quiesence(int alpha, int beta, Position& pos, SearchInfo& info)
{
	if ((info.nodes & 2047) == 0 && !info.infinite)
		check_up(info);

	if (info.stopped)
		return 0;

	info.nodes++;

	if(pos.fiftyply >= 100 || is_repetition(pos))
		return 0;
	
	int stand_pat { evaluate(pos) };
	
	
	if(pos.ply >= MAX_DEPTH)
		return stand_pat;

	if(stand_pat >= beta)
		return beta;
	
	if(alpha < stand_pat)
		alpha = stand_pat;
	
	Move captures[MAX_MOVES];
	Move* end = gen_captures(pos, pos.side, captures);
	int move_count { 0 };

	for (Move* p_move { captures }; p_move < end; p_move++)
	{
		next_move(p_move, end);
		Move& move { *p_move };

		if (!make_move(move, pos))
			continue;


		move_count++;
		int node { -quiesence(-beta, -alpha, pos, info) };
		pos.undo_move();

		if (info.stopped)
			return 0;

		if (node > alpha)
		{
			if (node >= beta)
			{
				if (move_count == 1)
					info.fhf++;

				info.fh++;

				return beta;
			}
			alpha = node;
		}
	}

	return alpha;
}

static int alphabeta(int alpha, int beta, int depth, Position& pos, SearchInfo& info, bool do_null)
{
	if ((info.nodes & 2047) == 0 && !info.infinite)
		check_up(info);

	if (info.stopped)
		return 0;

	if (depth <= 0)
	{
		info.nodes++;
		return quiesence(alpha, beta, pos, info);
	}
	info.nodes++;

	if (pos.fiftyply >= 100 || is_repetition(pos))
		return 0;
	else if (pos.ply >= MAX_DEPTH)
		return quiesence(alpha, beta, pos, info);

	bool in_check = is_in_check(pos, pos.side);

	if(in_check)
		depth++;
	
	if(do_null && !in_check && pos.bigpieces[pos.side] > 0 && depth >= 4)
	{
		pos.make_null();
		int node { -alphabeta(-beta, -beta + 1, depth - 4, pos, info, false) };
		pos.undo_null();

		if(info.stopped)
			return 0;
		
		if(node >= beta && std::abs(node) < MATE - MAX_DEPTH)
		{
			info.nullcut++;
			return beta;
		}
	}

	Move  moves[MAX_MOVES];
	Move* end = gen_pseudo(pos, pos.side, moves);

	index(moves, end, pos);

	int old_alpha { alpha };
	int legal_count { 0 };
	Move* bestmove { moves };

	// U64 hash { pos.hash() };

	for (Move* p_move = moves; p_move < end; p_move++)
	{
		next_move(p_move, end);
		Move& move { *p_move };

		if (!make_move(move, pos))
			continue;

		// It's legal
		legal_count++;

		int node { -alphabeta(-beta, -alpha, depth - 1, pos, info, true) };
		pos.undo_move();

		if (info.stopped)
			return 0;

		if (node > alpha)
		{
			if (node >= beta)
			{
				if (legal_count == 1)
					info.fhf++;

				info.fh++;

				if (move.capture() == NO_PIECE)
				{
					pos.search_killers[pos.ply][1] = pos.search_killers[pos.ply][0];
					pos.search_killers[pos.ply][0] = move;
				}
				return beta;
			}
			alpha = node;
			bestmove = &move;

			if (move.capture() == NO_PIECE)
				pos.history_heuristic[move.from()][move.to()] += depth;
		}
	}

	if (legal_count == 0)
	{
		if (in_check)
			return -MATE + pos.ply;
		else
			return 0;
	}
	
	if(alpha != old_alpha)
		store_pv(pos.hash(), *bestmove);

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
		for (int y { 0 }; y < SQUARE_COUNT; y++)
			pos.history_heuristic[x][y] = 0;

	for (int ply { 0 }; ply < MAX_DEPTH; ply++)
	{
		pos.search_killers[ply][0] = NO_MOVE;
		pos.search_killers[ply][1] = NO_MOVE;
	}
}

void search(SearchInfo* info_ptr, Position* pos_ptr, std::function<void(const ReportInfo& info)> report)
{
	/* Loop through moves
		-> Check move legality
		-> Make alpha beta search
		-> iterative deepening
	*/

	SearchInfo& info { *info_ptr };
	ReportInfo  rinfo {};

	Position& pos { *pos_ptr };

	init_search(info, pos);

	Move  moves[MAX_MOVES];
	Move* end = gen_pseudo(pos, pos.side, moves);
	index(moves, end, pos);
	std::sort(moves, end, [](const Move& before, const Move& after) { return before.score > after.score; });

	U64 hash { pos.hash() };
	long long int move_count { end - moves };

	for (size_t depth { 1 }; depth <= info.depth && !info.stopped; depth++)
	{
		info.last_new_depth = msclock();
		info.cur_depth = depth;
		info.progress = 0;
		info.end_estimate = info.end;

		Move* bestmove = moves;
		int   cp { -INFINITE };

		for (Move* move = moves; move < end; move++)
		{
			if (!make_move(*move, pos))
				continue;

			int score { -alphabeta(-INFINITE, -cp, depth - 1, pos, info, true) };
			if (score > cp)
			{
				cp	 = score;
				bestmove = move;

				store_pv(hash, *bestmove);
			}

			pos.undo_move();

			if (info.stopped)
				goto plotmove;

			long double searched_moves = move - moves + 1;
			long double searched_time = msclock() - info.last_new_depth;

			if(searched_time > 0)
			{
				long double speed {searched_time / searched_moves  };
				info.progress = searched_moves / move_count;
				info.end_estimate = info.start + speed * move_count;
			}
		}

		// Report intermediate result to the gui

		rinfo.bestmove = *bestmove;
		rinfo.time     = msclock() - info.start;
		rinfo.depth    = depth;
		rinfo.nodes    = info.nodes;

		rinfo.pv.clear();
		rinfo.pv.reserve(depth);

		if (MATE - std::abs(cp) <= MAX_DEPTH)
		{
			rinfo.score_type = "mate";
			rinfo.score	 = MATE - std::abs(cp);

			rinfo.score = (rinfo.score + 1) / 2;

			if (cp < 0)
				rinfo.score *= -1;
		}
		else
		{
			rinfo.score_type = "cp";
			rinfo.score	 = cp;
		}

		size_t pv { 0 };
		for (size_t i { 0 }; i < depth; i++)
		{
			Move bestmove { probe_pv(pos.hash()) };
			if (bestmove == NO_MOVE || !exists(bestmove, pos))
				break;
			if (!make_move(bestmove, pos))
				break;
			rinfo.pv.push_back(bestmove);
			pv++;
		}

		for (size_t i { 0 }; i < pv; i++)
		{
			pos.undo_move();
		}

		report(rinfo);
		std::cout << "Ordering: " << (float)info.fhf / info.fh << '\n';
		std::cout << "Null cutoffs: " << info.nullcut << '\n';

		if (info.stopped)
			break;
	}
plotmove:
	rinfo.finished = true;
	report(rinfo);
}