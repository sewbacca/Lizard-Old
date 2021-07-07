
#pragma once

#include "def.h"
#include "position.h"
#include "stack_vector.h"

constexpr int MAX_MOVES { 512 };
typedef stack_vector<Move, MAX_MOVES> MoveList;

bool make_move(Move pseudo_move, Position&);

// Pseudo movegenerator
void gen_pseudo(const Position&, Color side, MoveList& list);

void gen_silent(const Position&, Color side, MoveList& list);
void gen_captures(const Position&, Color side, MoveList& list);
