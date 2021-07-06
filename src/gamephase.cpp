
#include "gamephase.h"


GamePhase phase_of(const Position& pos)
{
	int pieces = 0;
	for(Piece p {WP}; p < PIECE_COUNT; p++ )
	{
		pieces += pos.piececount[p];
	}

	if(pieces == 32)
		return GamePhase::OPENING;
	
	if(pos.bigpieces[WHITE] <= 2 && pos.bigpieces[BLACK] <= 2 && pos.piececount[WQ] <= 0 && pos.piececount[BQ] <= 0)
		return GamePhase::END_GAME;
	
	return GamePhase::MIDDLE_GAME;

}