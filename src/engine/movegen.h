
#pragma once

#include "def.h"
#include "position.h"

// Pseudo movegenerator
Move* gen_pseudo(const Position&, Color side, Move* list);

Move* gen_silent(const Position&, Color side, Move *list);
Move* gen_captures(const Position&, Color side, Move *list);
