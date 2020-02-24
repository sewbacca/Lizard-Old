#include "attacks.h"

#include "bitboard.h"
#include "magic.h"

// Cache

bitboard KNIGHT_ATKS[SQUARE_COUNT];
bitboard KING_ATKS[SQUARE_COUNT];
bitboard PAWN_ATKS[2][SQUARE_COUNT];

// Indexer

bitboard attacks_p(Square sq, Color col) {
	return PAWN_ATKS[col][sq];
}

bitboard defenders_p(Square sq, Color col) {
	return PAWN_ATKS[swap(col)][sq];
}

bitboard attacks_n(Square sq) {
	return KNIGHT_ATKS[sq];
}

bitboard attacks_k(Square sq) {
	return KING_ATKS[sq];
}

bitboard attacks_b(Square sq, bitboard occupied) {
	Magic::MagicBitboard& w = Magic::MAGIC_BISHOPS[sq];
	return w.list[w.index(occupied)];
}

bitboard attacks_r(Square sq, bitboard occupied) {
	Magic::MagicBitboard& w = Magic::MAGIC_ROOKS[sq];
	return w.list[w.index(occupied)];
}

bitboard attacks_q(Square sq, bitboard occupied) {
	return attacks_b(sq, occupied) | attacks_r(sq, occupied);
}

bool is_attacked(Square sq, Color attacker, const Position& pos) {
	bitboard all = pos.pieces();

	bitboard dia = pos.pieces(attacker, QUEEN) | pos.pieces(attacker, BISHOP);
	bitboard hv = pos.pieces(attacker, QUEEN) | pos.pieces(attacker, ROOK);
	bitboard knights = pos.pieces(attacker, KNIGHT);
	bitboard pawns = pos.pieces(attacker, PAWN);
	bitboard kings = pos.pieces(attacker, KING);

	return dia & attacks_b(sq, all) ||
		hv & attacks_r(sq, all) ||
		knights & attacks_n(sq) ||
		pawns & defenders_p(sq, attacker) ||
		kings & attacks_k(sq);
}

bool is_in_check(const Position& pos, Color side) {
	Piece king = combine(side, KING);
	for(int i = 0; i < pos.piececount[king]; i++) {
		if(is_attacked(pos.piecepos[king][i], swap(side), pos))
			return true;
	}

	return false;
}

// Pre-calculating cache with attacks

size_t init_attacks() {
	auto offset = [](Square rel, int offX, int offY) -> bitboard {
		int x = file(rel), y = rank(rel);
		x += offX, y += offY;
		if(x < 0 || x >= BOARD_SIZE ||
			y < 0 || y >= BOARD_SIZE) return 0;
		
		return cell(idx(x, y));
	};

	auto generate_knight = [&offset](Square sq) -> bitboard {
		bitboard attacks = 0;

		attacks |= offset(sq,  1,  2 );
		attacks |= offset(sq, -1,  2 );
		attacks |= offset(sq,  1, -2 );
		attacks |= offset(sq, -1, -2 );
		attacks |= offset(sq,  2,  1 );
		attacks |= offset(sq, -2,  1 );
		attacks |= offset(sq,  2, -1 );
		attacks |= offset(sq, -2, -1 );

		return attacks;
	};

	auto generate_king = [&offset](Square sq) -> bitboard {
		bitboard attacks = 0;

		attacks |= offset(sq,  1,  1 );
		attacks |= offset(sq, -1,  1 );
		attacks |= offset(sq,  1, -1 );
		attacks |= offset(sq, -1, -1 );
		attacks |= offset(sq, -1,  0 );
		attacks |= offset(sq,  1,  0 );
		attacks |= offset(sq,  0,  1 );
		attacks |= offset(sq,  0, -1 );

		return attacks;
	};

	auto generate_pawn_atks = [&offset](Square sq, Color col) -> bitboard {
		bitboard attacks = 0;
		int dir = col == WHITE ? 1 : -1;

		attacks |= offset(sq, 1, dir);
		attacks |= offset(sq, -1, dir);

		return attacks;
	};
	
	for(Square sq = 0; is_inside(sq); sq++) {
		KNIGHT_ATKS[sq] = generate_knight(sq);
		KING_ATKS[sq] = generate_king(sq);
		PAWN_ATKS[WHITE][sq] = generate_pawn_atks(sq, WHITE);
		PAWN_ATKS[BLACK][sq] = generate_pawn_atks(sq, BLACK);
	}

	// Magic bitboard initialization

	return Magic::init_magic() +
		sizeof(KNIGHT_ATKS) +
		sizeof(KING_ATKS) +
		sizeof(PAWN_ATKS);
}