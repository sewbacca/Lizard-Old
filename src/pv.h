
#pragma once

#include "position.h"

size_t init_pv(size_t bytes);
void clear_pv();

Move probe_pv(U64 hash);
void store_pv(U64 hash, Move);