
#pragma once

#include "def.h"

class Position
{
private:
	bitboard wp{0}, wn{0}, wb{0}, wr{0}, wq{0}, wk{0};
	bitboard bp{0}, bn{0}, bb{0}, br{0}, bq{0}, bk{0};

	// Board representation
	bitboard* board() { return &wp; }
public:
	void set(int x, int y, Piece);
	Piece get(int x, int y);
};