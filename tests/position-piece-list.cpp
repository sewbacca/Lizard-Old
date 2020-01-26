
#include "catch.hpp"
#include "engine/position.h"

TEST_CASE("Position piece list", "[Position]") {
	Position p;

	p.set(8, WK);
	CHECK(p.pieces[WK][0] == 8);
	CHECK(p.piececount[WK] == 1);

	p.set(8, NO_PIECE);
	CHECK(p.piececount[WK] == 0);
}