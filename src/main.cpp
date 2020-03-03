
#include <iostream>
#include "init.h"
#include "textutils.h"
#include "search.h"

int main(int argc, char const *argv[])
{
	init();
	while (true) {
		char fen[255];
		std::cin.getline(fen, 255);
		Position pos = load_fen(fen);
		printf("fen: %s\n", fen);
		std::cout << to_string(pos) << std::endl;
		search(SearchInfo(), pos);
	}
}
