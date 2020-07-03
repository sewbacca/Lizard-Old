
#pragma once

#include <cstdint>
#include <cstdlib>

#include "enum_operator.h"

constexpr char* startpos { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };

typedef uint64_t U64;
typedef uint64_t bitboard;
typedef int Square;

constexpr int MAX_PIECES { 10 };
constexpr int BOARD_SIZE { 8 };
constexpr int SQUARE_COUNT { BOARD_SIZE * BOARD_SIZE };
constexpr int PIECE_TYPES { 12 };

enum Piece
{
	WP,
	WN,
	WB,
	WR,
	WQ,
	WK,
	BP,
	BN,
	BB,
	BR,
	BQ,
	BK,
	NO_PIECE
};

enum PieceType
{
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING,
	NO_PIECE_TYPE,
};

enum Color
{
	WHITE,
	BLACK,
};

enum CastlingSide
{
	KING_SIDE   = 0b1010,
	QUEEN_SIDE  = 0b0101,
	W_OO	    = 0b1000,
	W_OOO	    = 0b0100,
	B_OO	    = 0b0010,
	B_OOO	    = 0b0001,
	CS_WHITE    = 0b1100,
	CS_BLACK    = 0b0011,
	CS_ALL	    = 0b1111,
	NO_CASTLING = 0
};

class Move
{
      protected:
	/* Move representation
		0000 0000 0000 0000 0000 0000 0011 1111 -> From
		0000 0000 0000 0000 0000 1111 1100 0000 -> To
		0000 0000 0000 0000 1111 0000 0000 0000 -> Prom
		0000 0000 0000 1111 0000 0000 0000 0000 -> Moving
		0000 0000 1111 0000 0000 0000 0000 0000 -> Captured
		Flags:
		0000 0001 0000 0000 0000 0000 0000 0000 -> EP
		0000 0010 0000 0000 0000 0000 0000 0000 -> Double pawn push
		0011 1100 0000 0000 0000 0000 0000 0000 -> Castle
	*/
	int move { 0 };

      public:
	int score { 0 };

	Square from();
	Square to();
	Piece piece();
	Piece capture();
	Piece promotion();
	CastlingSide castling();

	bool isDoublePawnPush();
	bool isEnPassant();

	void setFrom(Square);
	void setTo(Square);
	void setCapture(Piece);
	void setPiece(Piece);
	void setPromotion(Piece);
	void setCastling(CastlingSide);

	void flagDoublePawnPush(bool);
	void flagEnPassant(bool);

	Move();

	friend class UndoMove;
	friend bool operator==(Move a, Move b);
};

const Move NO_MOVE { Move() };

constexpr bool is_inside(Square);
constexpr bool is_inside(int x, int y);
constexpr Color swap(Color);
constexpr Square idx(int x, int y);
constexpr int rank(Square);
constexpr int file(Square);
constexpr Color piece_col(Piece);
constexpr PieceType piece_type(Piece);
constexpr Piece combine(Color, PieceType);
inline bool operator==(Move a, Move b);

// Bitwise operator for CastlingSide

enum_operator_rhs(CastlingSide, int, |)
enum_operator_rhs(CastlingSide, int, &)
enum_operator_rhs(CastlingSide, int, ^)
enum_bitwise_operator_invert(CastlingSide, int, CastlingSide::CS_ALL)

enum_operator_lhs(CastlingSide, |=, |)
enum_operator_lhs(CastlingSide, &=, &)
enum_operator_lhs(CastlingSide, ^=, ^)

// Numeric operator for Piece

enum_operator_rhs(Piece, int, +)
enum_operator_rhs(Piece, int, -)

enum_operator_lhs(Piece, +=, +)
enum_operator_lhs(Piece, -=, -)

enum_prefix_operator(Piece, ++, +)

enum_postfix_operator(Piece, ++, +)

enum_prefix_operator(Piece, --, -)

enum_postfix_operator(Piece, --, -)

// Constexpr functions

constexpr bool is_inside(Square sq) { return sq >= 0 && sq < SQUARE_COUNT; }

constexpr bool is_inside(int x, int y) { return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE; }

constexpr Color swap(Color color) { return color == WHITE ? BLACK : WHITE; }

constexpr Square idx(int x, int y) { return y * BOARD_SIZE + x; }

constexpr int rank(Square sq)
{
	if (!is_inside(sq)) return -1;
	return sq / BOARD_SIZE;
}

constexpr int file(Square sq)
{
	if (!is_inside(sq)) return -1;
	return sq % BOARD_SIZE;
}

constexpr Color piece_col(Piece p) { return p < BP ? WHITE : BLACK; }

constexpr PieceType piece_type(Piece p)
{
	if (piece_col(p) == BLACK) p -= BP;
	return (PieceType)p;
}

constexpr Piece combine(Color col, PieceType p)
{
	Piece start { col == WHITE ? WP : BP };
	return (Piece)(start + p);
}

inline bool operator==(Move a, Move b) { return a.move == b.move; }