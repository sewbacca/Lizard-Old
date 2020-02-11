
#include "textutils.h"
#include "bitboard.h"

#include <sstream>

#include <cassert>
#include <cstring>
#include <ctype.h>
#include <cstdlib>

std::string to_string(const Position& p) {
	std::stringstream stream;

	static const char PIECES[] = {
		'P', 'N', 'B', 'R', 'Q', 'K',
		'p', 'n', 'b', 'r', 'q', 'k',
		'.'
	};

	for(int x = 0; x < BOARD_SIZE; x++)
		stream << "--";
	stream << "-\n";

	for(int y = BOARD_SIZE - 1; y >= 0; y--) {
		for(int x = 0; x < BOARD_SIZE; x++)
			stream << '|' << PIECES[p.get(idx(x, y))];
		stream  << "|\n";
	}

	for(int x = 0; x < BOARD_SIZE; x++)
		stream << "--";
	stream << "-\n";
	
	stream << "hisply: " << (p.hisply / 2) << '\n';
	stream << "fiftyply: " << p.fiftyply << '\n';
	
	stream << "rights: ";
	if(W_OO & p.rights)
		stream << 'K';
	if(W_OOO & p.rights)
		stream << 'Q';
	if(B_OO & p.rights)
		stream << 'k';
	if(B_OOO & p.rights)
		stream << 'q';
	stream << '\n';

	int index = lsb(p.enpassantsq);

	stream << "en pas: ";
	if(index != -1) {
		stream << ('a' + index % BOARD_SIZE);
		stream << ('0' + index / BOARD_SIZE);
	}

	stream << '\n';

	return stream.str();
}

static const char* skipfirstwhite(const char* c) {
	while(*c == ' ')
		c++;

	return c;
}

static const char* skipwhite(const char* c) {
	while(*c != ' ' && *c != '\0')
		c++;

	while(*c == ' ')
		c++;
	
	return c;
}

Position load_fen(const char* fen) {
	const char* c = skipfirstwhite(fen);
	Position result;

	// Board
	{
		int x = 0, y = 7;
		while(*c != ' ') {
			assert(isdigit(*c) || islower(*c) || isupper(*c) || *c == '/');

			if(isdigit(*c)) {		// Skip empty pieces
				x += *c - '0';
			} else if(*c == '/') {	// Next
				x = 0;
				y--;
			} else {
				Piece p = isupper(*c) ? WP : BP;
				switch(toupper(*c)) {
					case 'P': // Do nothing
						break;
					case 'N':
						p = (Piece) (p + 1);
						break;
					case 'B':
						p = (Piece) (p + 2);
						break;
					case 'R':
						p = (Piece) (p + 3);
						break;
					case 'Q':
						p = (Piece) (p + 4);
						break;
					case 'K':
						p = (Piece) (p + 5);
						break;
					default:
						assert(false);
				}

				result.set(idx(x, y), p);
				x++;
			}

			c++;
		}
	}

	{	// Color
		c = skipwhite(c);

		if(*c == 'w')
			result.side = WHITE;
		else if(*c == 'b')
			result.side = BLACK;
		else
			assert(false);
	}

	{	// Castling rights
		c = skipwhite(c);

		if(*c == '-')
			goto enpassantsq;

		while(*c != ' ') {
			assert(isupper(*c) || islower(*c));

			switch(*c) {
				case 'K':
					result.rights |= W_OO;
					break;
				case 'Q':
					result.rights |= W_OOO;
					break;
				case 'k':
					result.rights |= B_OO;
					break;
				case 'q':
					result.rights |= B_OOO;
					break;
				default:
					assert(false);
			}

			c++;
		}
	}

	{	enpassantsq:
		c = skipwhite(c);

		if (*c == '-')
			goto fiftymoverule;
		
		int x = *c - 'a'; c++;
		int y = *c - '1';

		result.enpassantsq = cell(idx(x, y));
	}

	{	fiftymoverule:
		c = skipwhite(c);

		result.fiftyply = atoi(c);
	}

	{	// Hisply
		c = skipwhite(c);

		result.hisply = atoi(c) * 2 + (result.side == WHITE ? 0 : 1) - 2;
	}

	return result;
}