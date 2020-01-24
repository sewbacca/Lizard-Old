
#include "catch.hpp"
#include "engine/position.h"

TEST_CASE("Position set and get", "[Position]") {
	Position pos;

	pos.set(1, 7, WP);
	pos.set(2, 6, WQ);

	CHECK(pos.get(1, 7) == WP);
	CHECK(pos.get(2, 6) == WQ);

	pos.set(2, 6, NO_PIECE);
	pos.set(1, 7, NO_PIECE);

	CHECK(pos.get(2, 6) == NO_PIECE);
	CHECK(pos.get(1, 7) == NO_PIECE);
}