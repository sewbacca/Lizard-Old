
#pragma once

#include "def.h"
#include "position.h"

size_t perft(size_t depth, Position& pos);
void perft_divide(size_t depth, Position& pos);
void step_through(size_t depth, Position& pos);
