
#include "catch.hpp"
#include "def.h"

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

TEST_CASE("Move encoding with to less information", "[Move]") {
	Move move;

	move.setPiece(WP);
	move.setFrom(56);
	move.setTo(12);
	move.flagDoublePawnPush(true);

	CHECK(move.capture() == NO_PIECE);
	CHECK(move.castling() == NO_CASTLING);
	CHECK(move.promotion() == NO_PIECE);
	CHECK(move.piece() == WP);
	CHECK(move.isEnPassant() == false);
	CHECK(move.from() == 56);
	CHECK(move.to() == 12);
}