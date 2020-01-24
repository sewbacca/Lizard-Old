
#pragma once

#include <string>
#include "position.h"

Position load_fen(const char* fen);
std::string to_string(Position);