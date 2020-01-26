#include "catch.hpp"
#include "engine/bitboard.h"

TEST_CASE("Bitboard set cell", "[Bitboard]") {
	bitboard test = Bitboard::cell(12);

	CHECK(Bitboard::is_set(test, 12));
}

TEST_CASE("Bitboard is set", "[Bitboard]") {
	bitboard test = 1ULL << 63;

	CHECK(Bitboard::is_set(test, 63));
}