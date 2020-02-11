#include "catch.hpp"
#include "engine/bitboard.h"

TEST_CASE("Bitboard set cell", "[Bitboard]") {
	bitboard test = cell(12);

	CHECK(is_set(test, 12));
}

TEST_CASE("Bitboard is set", "[Bitboard]") {
	bitboard test = 1ULL << 63;

	CHECK(is_set(test, 63));
}