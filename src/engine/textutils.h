
#pragma once

#include <string>
#include "position.h"

Position load_fen(const char* fen);
// Debugging purpose
std::string to_string(const Position&);
std::string to_string(bitboard);