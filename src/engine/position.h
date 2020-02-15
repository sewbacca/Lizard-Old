
#pragma once

#include "def.h"

class UndoMove : public Move {
public:
	CastlingSide before_rights = NO_CASTLING;
	bitboard before_enpassantsq = 0;
	int before_fiftyply = 0;

	UndoMove(Move move) { this->move = move.move; }
	UndoMove() { }
};

class Position
{
private:
	bitboard wp{0}, wn{0}, wb{0}, wr{0}, wq{0}, wk{0};
	bitboard bp{0}, bn{0}, bb{0}, br{0}, bq{0}, bk{0};

	U64 m_hash {0};

	UndoMove history[MAX_HISTORY];
public:
	Color side = WHITE;
	CastlingSide rights = NO_CASTLING;
	bitboard enpassantsq = 0;
	
	int fiftyply = 0;
	int hisply = 0;
	int ply = 0;

	bitboard* board() { return &wp; }
	const bitboard* board() const { return &wp; }
	bitboard wpieces() const { return wp | wn | wb | wr | wq | wk; }
	bitboard bpieces() const { return bp | bn | bb | br | bq | bk; }
	bitboard pieces() const { return wpieces() | bpieces(); }
	bitboard pieces(Color c) const { return c == WHITE ? wpieces() : bpieces(); }

	void set(Square, Piece);
	Piece get(Square) const;

	Square piecepos[PIECE_TYPES][MAX_PIECES];
	int piececount [PIECE_TYPES] {};

	U64 hash() const;

	void makeMove(Move);
	void undoMove();
};