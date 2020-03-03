
#pragma once

#include <string>
#include "position.h"

Position load_fen(const char* fen);
// Debugging purpose
std::string to_string(const Position&);
std::string to_string(bitboard);
std::string to_string(Move move);

// UCI Protocol
std::string to_uci(Move move);
Move from_uci(std::string move, const Position&);
