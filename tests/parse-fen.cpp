#include "catch.hpp"
#include "engine/textutils.h"

TEST_CASE("Parse a fen correctly", "[fen]") {
	Position result = load_fen(startpos);

	// Pawns
	int y = BOARD_SIZE - 2;
	for(int x = 0; x < BOARD_SIZE; x++) {
		CHECK(result.get(x, y) == BP);
	}

	y = 1;
	for(int x = 0; x < BOARD_SIZE; x++) {
		CHECK(result.get(x, y) == WP);
	}

	// Empty
	for(int y = 3; y < 6; y++) {
		for(int x = 0; x < BOARD_SIZE; x++)
			CHECK(result.get(x, y) == NO_PIECE);
	}
	
	// Rooks
	CHECK(result.get(0, 0) == WR);
	CHECK(result.get(7, 0) == WR);
	CHECK(result.get(0, 7) == BR);
	CHECK(result.get(7, 7) == BR);

	// Knight
	CHECK(result.get(1, 0) == WN);
	CHECK(result.get(6, 0) == WN);
	CHECK(result.get(1, 7) == BN);
	CHECK(result.get(6, 7) == BN);

	// Bishop
	CHECK(result.get(2, 0) == WB);
	CHECK(result.get(5, 0) == WB);
	CHECK(result.get(2, 7) == BB);
	CHECK(result.get(5, 7) == BB);

	// Bishop
	CHECK(result.get(2, 0) == WB);
	CHECK(result.get(5, 0) == WB);
	CHECK(result.get(2, 7) == BB);
	CHECK(result.get(5, 7) == BB);

	// King and Queen
	CHECK(result.get(4, 0) == WK);
	CHECK(result.get(3, 0) == WQ);
	CHECK(result.get(4, 7) == BK);
	CHECK(result.get(3, 7) == BQ);

	// Meta

	CHECK(result.hisply == 1);
	CHECK(result.fiftyply == 0);
	CHECK(result.side == WHITE);
	CHECK(result.rights == CS_ALL);
	CHECK(result.enpassantsq == 0);
}