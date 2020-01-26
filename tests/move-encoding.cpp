
#include "catch.hpp"
#include "engine/def.h"

TEST_CASE("Move encoding", "[Move]") {
	Move move;

	move.setCapture(WP);
	move.setCastling(W_OO);
	move.setPromotion(WK);
	move.flagEnPassant(true);
	move.setFrom(Point(0, 2));
	move.setTo(Point(4, 5));
	move.flagDoublePawnPush(false);

	CHECK(move.capture() == WP);
	CHECK(move.castling() == W_OO);
	CHECK(move.promotion() == WK);
	CHECK(move.isEnPassant() == true);
	CHECK(move.from().x == 0);
	CHECK(move.from().y == 2);
	CHECK(move.to().x == 4);
	CHECK(move.to().y == 5);
}