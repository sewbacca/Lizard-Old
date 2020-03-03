
#include <cassert>
#include <iostream>
#include "textutils.h"

#include "search.h"
#include "movegen.h"
#include "eval.h"
#include "attacks.h"
#include "pv.h"


static bool is_legal(Move move, Position& pos) {
	pos.makeMove(move);
	if(is_in_check(pos, swap(pos.side))) {
		pos.undoMove();
		return false;
	}


	return true;
}

static bool is_repetition(Position& pos) {
	U64 hash = pos.hash();

	for(int i = pos.hisply - 1; i >= (pos.hisply - pos.fiftyply); i--) {
		if(pos.history[i].hash == hash) {
			return true;
		}
	}

	return false;
}

int alphabeta(int alpha, int beta, int depth, Position& pos) {
	if(depth <= 0) {
		return evaluate(pos);
	} else if(is_repetition(pos)) {
		return 0;
	}

	Move moves [MAX_MOVES];
	Move* end = gen_pseudo(pos, pos.side, moves);


	int score = -INFINITE;
	int legal_moves = 0;

	U64 hash = pos.hash();

	for(Move* move = moves; move < end; move++) {
		if(!is_legal(*move, pos)) continue;
		int node = alphabeta(-beta, -alpha, depth - 1, pos);
		pos.undoMove();

		if(node > score) {
			score = node;
			store_pv(hash, *move);
			if(score > beta) {
				return score;
			}
		}

		legal_moves++;
	}

	if(legal_moves == 0) {
		if(is_in_check(pos, pos.side)) {
			return MATE - pos.ply;
		} else {
			return 0;
		}
	}

	return score;
}

void init_search() {
	clear_pv();
}

void search(const SearchInfo& info, Position& pos) {
	/* Loop through moves
		-> Check move legality
		-> Make alpha beta search
		-> iterative deepening
	*/

	Move moves [MAX_MOVES];
	Move* end = gen_pseudo(pos, pos.side, moves);
	size_t depth = info.depth;
	init_search();

	U64 hash = pos.hash();

	for(int i = 1; i < 7; i++) {
		Move* bestmove = moves;
		int alpha = -INFINITE;
		int beta = INFINITE;
		for(Move* move = moves; move < end; move++) {
			if(!is_legal(*move, pos)) continue;

			int score = alphabeta(-beta, -alpha, depth, pos);
			if(score > alpha) {
				alpha = score;
				bestmove = move;

				store_pv(hash, *bestmove);
			}

			pos.undoMove();
		}

		std::cout << "bestmove: " << to_string(*bestmove) << " depth: " << depth << " alpha: " << alpha << " beta: " << beta << " pv:";

		size_t pv = 0;
		for(size_t i = 0; i < depth; i++) {
			Move bestmove = probe_pv(pos.hash());
			if(!is_legal(bestmove, pos)) break;
			std::cout << ' ' << to_string(bestmove);
			pv++;
		}

		for(size_t i = 0; i < pv; i++) {
			pos.undoMove();
		}

		std::cout << std::endl;

		depth++;
	}
}