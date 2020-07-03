
#include "catch.hpp"
#include "pv.h"
#include "textutils.h"

TEST_CASE("Store and probe a move", "[pv]")
{
	Position pos { load_fen(startpos) };
	Move move { from_uci("e2e4", pos) };
	store_pv(pos.hash(), move);
	CHECK(probe_pv(pos.hash()) == move);
	pos.makeMove(move);
	move = from_uci("e7e5", pos);
	store_pv(pos.hash(), move);
	CHECK(probe_pv(pos.hash()) == move);
}