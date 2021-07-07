
#include "movegen.h"

#include "attacks.h"
#include "bitboard.h"
#include "bitboard_constants.h"

#include <cassert>
#include <iostream>
#include "textutils.h"

// All

bool make_move(Move pseudo_move, Position& pos)
{
	pos.make_move(pseudo_move);
	if (is_in_check(pos, swap(pos.side)))
	{
		pos.undo_move();
		return false;
	}

	return true;
}

void gen_pseudo(const Position& pos, Color side, MoveList& list)
{
	gen_captures(pos, side, list);
	gen_silent(pos, side, list);
}

// Helper

static void gen_pawn_captures(const Position& pos, Color side, MoveList& list);
static void gen_pawn_normal(const Position& pos, Color side, MoveList& list);
static void gen_king_silent(const Position& pos, Color side, MoveList& list);

static void add_capture(Square from, Square to, MoveList& list, const Position& pos);
static void add_silent(Square from, Square to, MoveList& list, const Position& pos);

// Details, details, details...

void gen_captures(const Position& pos, Color side, MoveList& list)
{
	bitboard all { pos.pieces() };

	gen_pawn_captures(pos, side, list);

	auto append_captures = [&](Piece piece, bitboard attacks(Square, bitboard)) -> void {
		for (int i { 0 }; i < pos.piececount[piece]; i++)
		{
			Square from { pos.piecepos[piece][i] };

			bitboard captures { pos.pieces(swap(side)) };

			captures &= attacks(from, all);

			while (captures)
			{
				Square to { pop_lsb(captures) };

				add_capture(from, to, list, pos);
			}
		}
	};

	Piece bishop { combine(side, BISHOP) };
	Piece knight { combine(side, KNIGHT) };
	Piece rook { combine(side, ROOK) };
	Piece queen { combine(side, QUEEN) };
	Piece king { combine(side, KING) };

	append_captures(bishop, attacks_b);
	append_captures(knight, [](Square s, bitboard) { return attacks_n(s); });
	append_captures(rook, attacks_r);
	append_captures(queen, attacks_q);
	append_captures(king, [](Square s, bitboard) { return attacks_k(s); });
}

void gen_silent(const Position& pos, Color side, MoveList& list)
{
	bitboard all { pos.pieces() };

	auto append = [&](Piece piece, bitboard attacks(Square, bitboard)) -> void {
		for (int i { 0 }; i < pos.piececount[piece]; i++)
		{
			Square from { pos.piecepos[piece][i] };

			bitboard non_captures { ~pos.pieces() };

			non_captures &= attacks(from, all);

			while (non_captures)
			{
				Square to { pop_lsb(non_captures) };

				add_silent(from, to, list, pos);
			}
		}
	};

	Piece bishop { combine(side, BISHOP) };
	Piece knight { combine(side, KNIGHT) };
	Piece rook { combine(side, ROOK) };
	Piece queen { combine(side, QUEEN) };

	append(bishop, attacks_b);
	append(knight, [](Square s, bitboard) { return attacks_n(s); });
	append(rook, attacks_r);
	append(queen, attacks_q);

	gen_king_silent(pos, side, list);
	gen_pawn_normal(pos, side, list);
}

void gen_king_silent(const Position& pos, Color side, MoveList& list)
{
	Piece	 king { combine(side, KING) };
	bitboard all { pos.pieces() };

	for (int i { 0 }; i < pos.piececount[king]; i++)
	{
		Square from { pos.piecepos[king][i] };

		bitboard non_captures { ~all };

		non_captures &= attacks_k(from);

		while (non_captures)
		{
			Square to { pop_lsb(non_captures) };

			add_silent(from, to, list, pos);
		}

		if (file(from) == 4 && !is_in_check(pos, side))
		{
			// Add castling
			CastlingSide OO { side == WHITE ? W_OO : B_OO };
			CastlingSide OOO { side == WHITE ? W_OOO : B_OOO };
			bitboard     rank_0 = side == WHITE ? RANK_1 : RANK_8;
			int	     y_0    = side == WHITE ? 0 : 7;

			if (pos.rights & OO)
			{
				bitboard free { rank_0 & (FILE_F | FILE_G) };
				Square	 walk_through { idx(5, y_0) };

				if ((free & all) == 0 && !is_attacked(walk_through, swap(side), pos))
				{
					Move castle { };

					castle.setFrom(from);
					castle.setTo(idx(6, y_0));
					castle.setPiece(king);
					castle.setCastling(OO);

					list.push_back(castle);
				}
			}
			if (pos.rights & OOO)
			{
				bitboard free { rank_0 & (FILE_B | FILE_C | FILE_D) };
				Square	 walk_through { idx(3, y_0) };

				if ((free & all) == 0 && !is_attacked(walk_through, swap(side), pos))
				{
					Move castle { };

					castle.setFrom(from);
					castle.setTo(idx(2, y_0));
					castle.setPiece(king);
					castle.setCastling(OOO);

					list.push_back(castle);
				}
			}
		}
	}
}

static void add_pawn_capture(Square from, Square to, Color side, MoveList& list, const Position&);
static void add_pawn_en_passant(Square from, Square to, Color side, MoveList& list, const Position&);

static void add_pawn_silent(Square to, Color side, MoveList& list, const Position&);
static void add_pawn_double_pawn_push(Square to, Color side, MoveList& list, const Position&);

static void gen_pawn_captures(const Position& pos, Color side, MoveList& list)
{
	Vertical dir { side == WHITE ? OFF_NORTH : OFF_SOUTH };

	auto append = [&](bitboard moves, int offX,
			      void add(Square from, Square to, Color side, MoveList& list, const Position&)) -> void {
		while (moves)
		{
			Square to { pop_lsb(moves) };
			Square from { to - idx(offX, dir) };

			add(from, to, side, list, pos);
		}
	};

	bitboard pawns { pos.pieces(side, PAWN) };

	bitboard captures_west { shift(pawns, OFF_WEST, dir) };
	bitboard captures_east { shift(pawns, OFF_EAST, dir) };
	bitboard enemys { pos.pieces(swap(side)) };

	append(captures_west & enemys, OFF_WEST, add_pawn_capture);
	append(captures_east & enemys, OFF_EAST, add_pawn_capture);
	append(captures_west & pos.enpassantsq, OFF_WEST, add_pawn_en_passant);
	append(captures_east & pos.enpassantsq, OFF_EAST, add_pawn_en_passant);
}

static void gen_pawn_normal(const Position& pos, Color side, MoveList& list)
{
	Vertical dir { side == WHITE ? OFF_NORTH : OFF_SOUTH };

	auto append = [&](bitboard moves, void add(Square to, Color side, MoveList& list, const Position&)) -> void {
		while (moves)
		{
			Square to { pop_lsb(moves) };

			add(to, side, list, pos);
		}
	};

	bitboard pawns { pos.pieces(side, PAWN) };
	bitboard blocker { pos.pieces() };

	bitboard rank_0 = side == WHITE ? RANK_2 : RANK_7;

	bitboard normal, double_pawns;
	if (dir == 1)
	{
		normal	     = pawns << idx(0, 1) & ~blocker;
		double_pawns = (pawns & rank_0) << idx(0, 2) & ~blocker << idx(0, 1) & ~blocker;
	}
	else
	{
		normal	     = pawns >> idx(0, 1) & ~blocker;
		double_pawns = (pawns & rank_0) >> idx(0, 2) & ~blocker >> idx(0, 1) & ~blocker;
	}

	append(normal, add_pawn_silent);
	append(double_pawns, add_pawn_double_pawn_push);
}

static void add_capture(Square from, Square to, MoveList& list, const Position& pos)
{
	Move move { };

	move.setFrom(from);
	move.setTo(to);
	move.setPiece(pos.get(from));
	move.setCapture(pos.get(to));

	list.push_back(move);
}

static void add_silent(Square from, Square to, MoveList& list, const Position& pos)
{
	Move move { };

	move.setFrom(from);
	move.setTo(to);
	move.setPiece(pos.get(from));

	list.push_back(move);
}
static Move create(Square from, Square to, Color side, Piece capture, Piece promotion, bool enpassant = false,
	bool doublepush = false)
{
	assert(promotion != WP && promotion != BP);
	Move res;

	res.setFrom(from);
	res.setTo(to);
	res.setPiece(combine(side, PAWN));
	res.setCapture(capture);
	res.setPromotion(promotion);

	res.flagEnPassant(enpassant);
	res.flagDoublePawnPush(doublepush);

	return res;
}

static void add_pawn_capture(Square from, Square to, Color side, MoveList& list, const Position& pos)
{
	bool isProm { side == WHITE ? rank(to) == 7 : rank(to) == 0 };

	if (!isProm)
	{
		list.push_back(create(from, to, side, pos.get(to), NO_PIECE));
	}
	else
	{
		list.push_back(create(from, to, side, pos.get(to), combine(side, QUEEN)));
		list.push_back(create(from, to, side, pos.get(to), combine(side, ROOK)));
		list.push_back(create(from, to, side, pos.get(to), combine(side, BISHOP)));
		list.push_back(create(from, to, side, pos.get(to), combine(side, KNIGHT)));
	}
}

static void add_pawn_en_passant(Square from, Square to, Color side, MoveList& list, const Position&)
{
	bool isProm { side == WHITE ? rank(to) == 7 : rank(to) == 0 };

	if (!isProm)
	{
		list.push_back(create(from, to, side, combine(swap(side), PAWN), NO_PIECE, true, false));
	}
	else
	{
		list.push_back(create(from, to, side, combine(swap(side), PAWN), combine(side, QUEEN), true, false));
		list.push_back(create(from, to, side, combine(swap(side), PAWN), combine(side, ROOK), true, false));
		list.push_back(create(from, to, side, combine(swap(side), PAWN), combine(side, BISHOP), true, false));
		list.push_back(create(from, to, side, combine(swap(side), PAWN), combine(side, KNIGHT), true, false));
	}
}

static void add_pawn_silent(Square to, Color side, MoveList& list, const Position&)
{
	Vertical dir { side == WHITE ? OFF_NORTH : OFF_SOUTH };
	bool	 isProm { side == WHITE ? rank(to) == 7 : rank(to) == 0 };

	Square from { to + idx(0, -dir) };

	if (!isProm)
	{
		list.push_back(create(from, to, side, NO_PIECE, NO_PIECE));
	}
	else
	{
		list.push_back(create(from, to, side, NO_PIECE, combine(side, QUEEN)));
		list.push_back(create(from, to, side, NO_PIECE, combine(side, ROOK)));
		list.push_back(create(from, to, side, NO_PIECE, combine(side, BISHOP)));
		list.push_back(create(from, to, side, NO_PIECE, combine(side, KNIGHT)));
	}
}

static void add_pawn_double_pawn_push(Square to, Color side, MoveList& list, const Position&)
{
	Vertical dir { side == WHITE ? OFF_NORTH : OFF_SOUTH };
	list.push_back(create(to + idx(0, -2 * dir), to, side, NO_PIECE, NO_PIECE, false, true));
}
