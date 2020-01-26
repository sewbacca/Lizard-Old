
#include "catch.hpp"
#include "engine/def.h"

TEST_CASE("Move encoding", "[Move]") {
	Move move;

	move.setPiece(WQ);
	move.setCapture(WP);
	move.setCastling(W_OO);
	move.setPromotion(WK);
	move.flagEnPassant(true);
	move.setFrom(56);
	move.setTo(12);
	move.flagDoublePawnPush(false);

	CHECK(move.capture() == WP);
	CHECK(move.castling() == W_OO);
	CHECK(move.promotion() == WK);
	CHECK(move.piece() == WQ);
	CHECK(move.isEnPassant() == true);
	CHECK(move.from() == 56);
	CHECK(move.to() == 12);
}