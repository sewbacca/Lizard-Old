
#include <cassert>
#include <algorithm>

#include "position.h"
#include "bitboard.h"
#include "hash.h"

Piece Position::get(Square sq) {
	assert(is_inside(sq));

	auto board = this->board();
	for(int p = 0; p < PIECE_TYPES; p++) {
		if(Bitboard::is_set(board[p], sq))
			return (Piece) p;
	}

	return NO_PIECE;
}

void Position::set(Square sq, Piece p) {
	assert(is_inside(sq));
	
	bitboard* board = this->board();
	Piece onboard = get(sq);

	// Delete piece
	if(onboard != NO_PIECE) {
		board[onboard] -= Bitboard::cell(sq);

		// Find piece
		for(int i = 0; i < piececount[onboard]; i++) {
			Square* pos = &pieces[onboard][i];
			if(*pos == sq) {
				std::swap(*pos, *(pieces[onboard] + piececount[onboard] - 1));
				piececount[onboard]--;
				break;
			}
		}
		
		m_hash ^= get_hash(sq, onboard);
	}

	// Add piece
	if(p != NO_PIECE) {
		
		board[p] |= Bitboard::cell(sq);
		pieces[p][piececount[p]] = sq;
		piececount[p]++;

		m_hash ^= get_hash(sq, p);
	}
}

U64 Position::hash() {
	return m_hash ^
		get_hash(Bitboard::index(enpassantsq) % BOARD_SIZE) ^
		get_hash(side) ^
		get_hash(rights);
}

constexpr Square POS_WR1 = sqr(0, 0);
constexpr Square POS_WR2 = sqr(7, 0);
constexpr Square POS_BR1 = sqr(0, 7);
constexpr Square POS_BR2 = sqr(7, 7);
constexpr Square POS_WK = sqr(4, 0);
constexpr Square POS_BK = sqr(4, 7);

void Position::makeMove(Move move) {
	// Store for restore
	history[hisply] = UndoMove(move);
	history[hisply].before_enpassantsq = enpassantsq;
	history[hisply].before_rights = rights;
	history[hisply].before_fiftyply = fiftyply;

	Piece rook = side == WHITE ? WR : BR;
	Piece king = side == WHITE ? WK : BK;

	// May be resetted
	enpassantsq = 0;
	fiftyply++;

	if(move.castling() == NO_CASTLING) {
		int p_dir = side == WHITE ? 1 : -1;
		Piece pawn = side == WHITE ? WP : BP;

		Piece piece = move.piece();

		if(move.piece() == rook) {
			CastlingSide OO = side == WHITE ? W_OO : B_OO;
			CastlingSide OOO = side == WHITE ? W_OOO : B_OOO;

			if(move.from() % BOARD_SIZE == 0) {
				rights &= ~OO;
			} else if (move.from() % BOARD_SIZE == 7) {
				rights &= ~OOO;
			}
		} else if (move.piece() == pawn) {
			if(move.isDoublePawnPush()) {
				enpassantsq = Bitboard::cell(move.to() - p_dir * 8);
			} else if (move.isEnPassant()) {
				set(move.to() - p_dir * 8, NO_PIECE);
			} else if (move.promotion() != NO_PIECE) {
				piece = move.promotion();
			}

			fiftyply = 0;
		} else if (move.piece() == king) {
			rights &= ~(side == WHITE ? CS_WHITE : CS_BLACK);
		}

		set(move.from(), NO_PIECE);
		set(move.to(), piece);
	} else {
		Square pos_k = side == WHITE ? POS_WK : POS_BK;
		Square pos_r = sqr(
			move.castling() & QUEEN_SIDE ? 0 : 7,
			side == WHITE ? 0 : 7
		);
		
		int cs_dir = move.castling() & KING_SIDE ? 1 : -1;

		set(pos_r, NO_PIECE);
		set(pos_k, NO_PIECE);
		set(pos_k + cs_dir * 2, king);
		set(pos_k + cs_dir * 1, rook);
	}

	hisply++;
	side = swap(side);
}


void Position::undoMove() {
	assert(hisply > 0);
	
	side = swap(side);

	UndoMove undomove = history[--hisply];

	Piece rook = side == WHITE ? WR : BR;
	Piece king = side == WHITE ? WK : BK;

	if(undomove.castling() == NO_CASTLING) {
		int p_dir = side == WHITE ? 1 : -1;
		
		set(undomove.from(), undomove.piece());
		
		if (undomove.isEnPassant()) {
			set(undomove.to() - p_dir * 8, undomove.capture());
			set(undomove.to(), NO_PIECE);
		} else
			set(undomove.to(), undomove.capture());
	} else {
		Square pos_k = side == WHITE ? POS_WK : POS_BK;
		Square pos_r = sqr(
			undomove.castling() & QUEEN_SIDE ? 0 : 7,
			side == WHITE ? 0 : 7
		);
		
		int cs_dir = undomove.castling() & KING_SIDE ? 1 : -1;

		set(pos_r, rook);
		set(pos_k, king);
		set(pos_k + cs_dir * 2, NO_PIECE);
		set(pos_k + cs_dir * 1, NO_PIECE);
	}

	fiftyply = undomove.before_fiftyply;
	rights = undomove.before_rights;
	enpassantsq = undomove.before_enpassantsq;
}