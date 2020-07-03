#include "catch.hpp"
#include "textutils.h"

#define index(x, y) ((x) + (y)*BOARD_SIZE)

TEST_CASE("Parse a fen correctly", "[fen]")
{
	Position result { load_fen(startpos) };

	// Pawns
	int y { BOARD_SIZE - 2 };
	for (int x { 0 }; x < BOARD_SIZE; x++)
	{
		CHECK(result.get(index(x, y)) == BP);
	}

	y = 1;
	for (int x { 0 }; x < BOARD_SIZE; x++)
	{
		CHECK(result.get(index(x, y)) == WP);
	}

	// Empty
	for (int y { 3 }; y < 6; y++)
	{
		for (int x { 0 }; x < BOARD_SIZE; x++)
			CHECK(result.get(index(x, y)) == NO_PIECE);
	}

	// Rooks
	CHECK(result.get(index(0, 0)) == WR);
	CHECK(result.get(index(7, 0)) == WR);
	CHECK(result.get(index(0, 7)) == BR);
	CHECK(result.get(index(7, 7)) == BR);

	// Knight
	CHECK(result.get(index(1, 0)) == WN);
	CHECK(result.get(index(6, 0)) == WN);
	CHECK(result.get(index(1, 7)) == BN);
	CHECK(result.get(index(6, 7)) == BN);

	// Bishop
	CHECK(result.get(index(2, 0)) == WB);
	CHECK(result.get(index(5, 0)) == WB);
	CHECK(result.get(index(2, 7)) == BB);
	CHECK(result.get(index(5, 7)) == BB);

	// Bishop
	CHECK(result.get(index(2, 0)) == WB);
	CHECK(result.get(index(5, 0)) == WB);
	CHECK(result.get(index(2, 7)) == BB);
	CHECK(result.get(index(5, 7)) == BB);

	// King and Queen
	CHECK(result.get(index(4, 0)) == WK);
	CHECK(result.get(index(3, 0)) == WQ);
	CHECK(result.get(index(4, 7)) == BK);
	CHECK(result.get(index(3, 7)) == BQ);

	// Meta

	CHECK(result.hisply == 0);
	CHECK(result.fiftyply == 0);
	CHECK(result.side == WHITE);
	CHECK(result.rights == CS_ALL);
	CHECK(result.enpassantsq == 0);
}