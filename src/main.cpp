
#include <iostream>
#include "engine/init.h"
#include "engine/attacks.h"
#include "engine/textutils.h"

int main(int argc, char const *argv[])
{
	init();

	Position pos = load_fen("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 1 2");
	
	std::cout << to_string(pos) << std::endl;
	std::cout << to_string(attacks_b(idx(5, 0), pos.pieces()));
	return 0;
}
