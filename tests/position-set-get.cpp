
#include "catch.hpp"
#include "position.h"

TEST_CASE("Position set and get", "[Position]") {
	Position pos;

	pos.set(63 - 7, WP);
	pos.set(2 + 6 * 8, WQ);

	CHECK(pos.get(63 - 7) == WP);
	CHECK(pos.get(2 + 6 * 8) == WQ);

	pos.set(63 - 7, NO_PIECE);
	pos.set(2 + 6 * 8, NO_PIECE);

	CHECK(pos.get(63 - 7) == NO_PIECE);
	CHECK(pos.get(2 + 6 * 8) == NO_PIECE);
}