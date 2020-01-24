
#include "fen.h"
#include "bitboard.h"

#include <cassert>
#include <cstring>
#include <ctype.h>
#include <cstdlib>

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

Position loadfen(const char* fen) {
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

				result.set(x, y, p);
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

		result.enpassantsq = Bitboard::cell(x, y);
	}

	{	fiftymoverule:
		c = skipwhite(c);

		result.fiftyply = atoi(c);
	}

	{	// Hisply
		c = skipwhite(c);

		result.hisply = atoi(c);
	}

	return result;
}