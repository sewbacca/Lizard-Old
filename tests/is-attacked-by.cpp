
#include "catch.hpp"

#include "textutils.h"
#include "attacks.h"

TEST_CASE("Is square attacked by color", "[attacks]")
{
	Position p { load_fen("8/8/1K2q3/8/3Q4/4k3/8/8 w - - 0 1") };

	CHECK(is_attacked(idx(1, 5), BLACK, p));
	CHECK(is_attacked(idx(2, 4), WHITE, p));

	CHECK_FALSE(is_attacked(idx(1, 2), WHITE, p));
	CHECK_FALSE(is_attacked(idx(0, 5), BLACK, p));
}