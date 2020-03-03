
#pragma once

#include <cstddef>

#include "position.h"

constexpr int MATE = 3000;
constexpr int INFINITE = 3100;

struct SearchInfo {
	size_t depth = 1;
	bool running = true;
};

void search(const SearchInfo&, Position& pos);