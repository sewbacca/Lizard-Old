
#pragma once

#include "position.h"

enum class GamePhase
{
	OPENING,
	MIDDLE_GAME,
	END_GAME
};

GamePhase phase_of(const Position& pos);