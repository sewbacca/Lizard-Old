
#pragma once

#include "def.h"

constexpr int MAX_HISTORY { 2048 };

class UndoMove : public Move
{
      public:
	CastlingSide before_rights { NO_CASTLING };
	CastlingSide before_done_castles { NO_CASTLING };
	bitboard before_enpassantsq { 0 };
	int before_fiftyply { 0 };

	U64 hash;
	UndoMove(Move move) { this->move = move.move; }
	UndoMove() {}
};

class Position
{
      private:
	bitboard wp { 0 }, wn { 0 }, wb { 0 }, wr { 0 }, wq { 0 }, wk { 0 };
	bitboard bp { 0 }, bn { 0 }, bb { 0 }, br { 0 }, bq { 0 }, bk { 0 };

	U64 m_hash { 0 };

      public:
	UndoMove history[MAX_HISTORY];
	Color side { WHITE };
	CastlingSide rights { NO_CASTLING };
	CastlingSide done_castles { NO_CASTLING };

	bitboard enpassantsq { 0 };

	int fiftyply { 0 };
	int hisply { 0 };
	// The ply from where we are searching
	int ply { 0 };

	bitboard* board() { return &wp; }
	const bitboard* board() const { return &wp; }
	bitboard wpieces() const { return wp | wn | wb | wr | wq | wk; }
	bitboard bpieces() const { return bp | bn | bb | br | bq | bk; }

	bitboard pieces() const { return wpieces() | bpieces(); }
	bitboard pieces(Piece p) const { return *(&wp + p); }
	bitboard pieces(Color c) const { return c == WHITE ? wpieces() : bpieces(); }
	bitboard pieces(Color col, PieceType t) const { return pieces(combine(col, t)); }

	void set(Square, Piece);
	Piece get(Square) const;

	Square piecepos[PIECE_COUNT][MAX_PIECES];
	int piececount[PIECE_COUNT] {};
	int bigpieces[2] {};

	Move search_killers[MAX_HISTORY][2]		  = {};
	int history_heuristic[SQUARE_COUNT][SQUARE_COUNT] = {};

	U64 hash() const;

	void make_move(Move);
	void undo_move();

	void make_null();
	void undo_null();
};