#include "catch.hpp"
#include "engine/position.h"
#include "engine/textutils.h"
#include "engine/bitboard.h"

constexpr int sq(const char *s) {
	return (s[0] - 'a' + (s[1] - '1') * BOARD_SIZE);
}

TEST_CASE("Make move double jump", "[position]") {
	Position start = load_fen(startpos);

	Move move;

	move.setFrom(sq("e2"));
	move.setTo(sq("e4"));
	move.flagDoublePawnPush(true);
	move.setPiece(WP);

	start.makeMove(move);

	CHECK(start.get(sq("e2")) == NO_PIECE);
	CHECK(start.get(sq("e4")) == WP);
	CHECK(start.hisply == 1);
	CHECK(start.fiftyply == 0);
	CHECK(start.enpassantsq == Bitboard::cell(20));
	CHECK(start.side == BLACK);
}

TEST_CASE("Make move en passant", "[position]") {
	Position pos = load_fen("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");

	Move move;

	move.setFrom(sq("e5"));
	move.setTo(sq("f6"));
	move.flagEnPassant(true);
	move.setPiece(WP);

	pos.makeMove(move);

	CHECK(pos.get(sq("e5")) == NO_PIECE);
	CHECK(pos.get(sq("f6")) == WP);
	CHECK(pos.get(sq("f5")) == NO_PIECE);
	CHECK(pos.hisply == 3);
	CHECK(pos.fiftyply == 0);
	CHECK(pos.enpassantsq == 0);
	CHECK(pos.side == BLACK);
}

TEST_CASE("Make move promotion", "[position]") {
	Position pos = load_fen("rnbq1bnr/pppkpP1p/6p1/3p4/8/8/PPPP1PPP/RNBQKBNR w KQ - 1 5");

	Move move;

	move.setFrom(sq("f7"));
	move.setTo(sq("g8"));
	move.setPromotion(WQ);
	move.setPiece(WP);

	pos.makeMove(move);

	CHECK(pos.get(sq("f7")) == NO_PIECE);
	CHECK(pos.get(sq("g8")) == WQ);
	CHECK(pos.fiftyply == 0);
	CHECK(pos.hisply == 5);
	CHECK(pos.enpassantsq == 0);
	CHECK(pos.side == BLACK);
}

TEST_CASE("Make move destroy castling", "[position]") {
	Position pos = load_fen("rnbqkbnr/1pP3p1/4p3/p4p1p/P6P/8/1PPP1PP1/RNBQKBNR b KQkq - 0 6");

	CHECK(pos.rights == CS_ALL);

	SECTION("with king") {
		Move move;

		move.setFrom(sq("e8"));
		move.setTo(sq("e7"));
		move.setPiece(BK);

		pos.makeMove(move);

		CHECK(pos.get(sq("e8")) == NO_PIECE);
		CHECK(pos.get(sq("e7")) == BK);
		CHECK(pos.fiftyply == 1);
		CHECK(pos.rights == CS_WHITE);
		CHECK(pos.hisply == 7);
	}

	SECTION("with a-rook") {
		Move move;

		move.setFrom(sq("a8"));
		move.setTo(sq("a7"));
		move.setPiece(BR);

		pos.makeMove(move);

		CHECK(pos.get(sq("a8")) == NO_PIECE);
		CHECK(pos.get(sq("a7")) == BR);
		CHECK(pos.fiftyply == 1);
		CHECK(pos.rights == (CS_ALL & ~B_OO));
		CHECK(pos.hisply == 7);
	}

	SECTION("with h-rook") {
		Move move;

		move.setFrom(sq("h8"));
		move.setTo(sq("h7"));
		move.setPiece(BR);

		pos.makeMove(move);

		CHECK(pos.get(sq("h8")) == NO_PIECE);
		CHECK(pos.get(sq("h7")) == BR);
		CHECK(pos.fiftyply == 1);
		CHECK(pos.rights == (CS_ALL & ~B_OOO));
		CHECK(pos.hisply == 7);
	}
}

TEST_CASE("Make move castling", "[Position]") {
	Position pos = load_fen("r3k2r/1pqb2p1/2n1pn2/pN3p1p/Pb5P/2P2NP1/1P1P1P2/R1BQKB1R b KQkq - 0 11");

	SECTION("OO") {
		Move move;

		move.setFrom(sq("e8"));
		move.setTo(sq("h8"));
		move.setCastling(B_OO);

		pos.makeMove(move);

		CHECK(pos.get(sq("h8")) == NO_PIECE);
		CHECK(pos.get(sq("g8")) == BK);
		CHECK(pos.get(sq("f8")) == BR);
		CHECK(pos.get(sq("e8")) == NO_PIECE);
	}

	SECTION("OOO") {
		Move move;

		move.setFrom(sq("e8"));
		move.setTo(sq("a8"));
		move.setCastling(B_OOO);

		pos.makeMove(move);

		CHECK(pos.get(sq("e8")) == NO_PIECE);
		CHECK(pos.get(sq("d8")) == BR);
		CHECK(pos.get(sq("c8")) == BK);
		CHECK(pos.get(sq("b8")) == NO_PIECE);
		CHECK(pos.get(sq("a8")) == NO_PIECE);
	}
}