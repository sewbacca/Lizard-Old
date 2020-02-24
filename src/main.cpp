
#include <iostream>
#include "engine/init.h"
#include "engine/perft.h"
#include "engine/textutils.h"

void perft(int depth, Position& pos) {
	printf("Searching depth: %d\n", depth);
	perft_divide(depth, pos);
}

int main(int argc, char const *argv[])
{
	init();
	while (true) {
		char fen[255];
		std::cin.getline(fen, 255);
		Position pos = load_fen(fen);
		printf("fen: %s\n", fen);
		std::cout << to_string(pos) << std::endl;
		std::string cmd;
		while(cmd != "depth") {
			std::cin >> cmd;
			if(cmd == "depth") break;
			pos.makeMove(from_uci(cmd, pos));
			std::cout << to_string(pos) << std::endl;

		}
		int depth = 0;
		std::cin >> depth;
		perft(depth, pos);
	}
}
