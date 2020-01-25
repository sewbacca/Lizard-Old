
#include "catch.hpp"
#include "engine/position.h"

TEST_CASE("Position piece list", "[Position]") {
	Position p;

	p.set(1, 1, WK);
	CHECK(p.pieces[WK][0].x == 1);
	CHECK(p.pieces[WK][0].y == 1);
	CHECK(p.piececount[WK] == 1);

	p.set(1, 1, NO_PIECE);
	CHECK(p.piececount[WK] == 0);
}