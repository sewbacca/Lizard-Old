#include <cassert>
#include "def.h"
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

// clang-format off

static constexpr int MASK_FROM     { 0b0000'0000'0000'0000'0000'0000'0011'1111 };
static constexpr int MASK_TO       { 0b0000'0000'0000'0000'0000'1111'1100'0000 };
static constexpr int MASK_PROM     { 0b0000'0000'0000'0000'1111'0000'0000'0000 };
static constexpr int MASK_MOV      { 0b0000'0000'0000'1111'0000'0000'0000'0000 };
static constexpr int MASK_CAP      { 0b0000'0000'1111'0000'0000'0000'0000'0000 };
static constexpr int MASK_EP       { 0b0000'0001'0000'0000'0000'0000'0000'0000 };
static constexpr int MASK_DPP      { 0b0000'0010'0000'0000'0000'0000'0000'0000 };
static constexpr int MASK_CASTLING { 0b0011'1100'0000'0000'0000'0000'0000'0000 };

static constexpr int OFF_FROM      { 0 };
static constexpr int OFF_TO        { 6 };
static constexpr int OFF_PROM      { 12 };
static constexpr int OFF_MOV       { 16 };
static constexpr int OFF_CAP       { 20 };
static constexpr int OFF_EP        { 24 };
static constexpr int OFF_DPP       { 25 };
static constexpr int OFF_CASTLING  { 26 };

// clang-format on

// Standard constructor

Move::Move()
{
	setCapture(NO_PIECE);
	setPiece(NO_PIECE);
	setPromotion(NO_PIECE);
}

// Getter

Square Move::from() const { return Square((move & MASK_FROM) << OFF_FROM); }

Square Move::to() const { return Square((move & MASK_TO) >> OFF_TO); }

Piece Move::promotion() const { return Piece((move & MASK_PROM) >> OFF_PROM); }

Piece Move::piece() const { return Piece((move & MASK_MOV) >> OFF_MOV); }

Piece Move::capture() const { return Piece((move & MASK_CAP) >> OFF_CAP); }

CastlingSide Move::castling() const { return CastlingSide((move & MASK_CASTLING) >> OFF_CASTLING); }

bool Move::isEnPassant() const { return (move & MASK_EP) >> OFF_EP; }

bool Move::isDoublePawnPush() const { return (move & MASK_DPP) >> OFF_DPP; }

// Setter

void Move::setFrom(Square Square)
{
	move &= ~MASK_FROM;
	move |= Square << OFF_FROM;
}

void Move::setTo(Square Square)
{
	move &= ~MASK_TO;
	move |= Square << OFF_TO;
}

void Move::setPromotion(Piece p)
{
	assert(piece_type(p) != PAWN);
	move &= ~MASK_PROM;
	move |= p << OFF_PROM;
}

void Move::setPiece(Piece p)
{
	move &= ~MASK_MOV;
	move |= p << OFF_MOV;
}

void Move::setCapture(Piece p)
{
	move &= ~MASK_CAP;
	move |= p << OFF_CAP;
}

void Move::setCastling(CastlingSide c)
{
	move &= ~MASK_CASTLING;
	move |= c << OFF_CASTLING;
}

void Move::flagEnPassant(bool ep)
{
	move &= ~MASK_EP;
	move |= ep << OFF_EP;
}

void Move::flagDoublePawnPush(bool dpp)
{
	move &= ~MASK_DPP;
	move |= dpp << OFF_DPP;
}