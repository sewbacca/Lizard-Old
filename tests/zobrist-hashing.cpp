#include "catch.hpp"
#include "position.h"
#include "textutils.h"

TEST_CASE("Position hashing", "[posistion] [hash]") {
	Position pos1 = load_fen(startpos);
	Position pos2 = load_fen(startpos);

	CHECK(pos1.hash() == pos2.hash());

	pos1.set(0, NO_PIECE);

	CHECK(pos1.hash() != pos2.hash());

	pos2.set(0, NO_PIECE);

	CHECK(pos1.hash() == pos2.hash());

	pos1.side = BLACK;

	CHECK(pos1.hash() != pos2.hash());

	pos2.side = BLACK;
	pos2.set(0, WR);

	CHECK(pos1.hash() != pos2.hash());

	pos1.set(0, WR);

	CHECK(pos1.hash() == pos2.hash());
}