
#include "textutils.h"
#include "bitboard.h"
#include "bitboard_constants.h"
#include "except.h"

#include <sstream>
#include <iostream>

#include <cassert>
#include <cmath>
#include <cstring>
#include <ctype.h>
#include <cstdlib>

const char PIECES[] = {
	'P',
	'N',
	'B',
	'R',
	'Q',
	'K',
};

std::string to_string(const Position& p)
{
	std::stringstream stream;

	static const char PIECES[] = { 'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', '.' };

	for (int x { 0 }; x < BOARD_SIZE; x++)
		stream << "--";
	stream << "-\n";

	for (int y { BOARD_SIZE - 1 }; y >= 0; y--)
	{
		for (int x { 0 }; x < BOARD_SIZE; x++)
			stream << '|' << PIECES[p.get(idx(x, y))];
		stream << "|\n";
	}

	for (int x { 0 }; x < BOARD_SIZE; x++)
		stream << "--";
	stream << "-\n";

	stream << "hisply: " << (p.hisply / 2) << '\n';
	stream << "fiftyply: " << p.fiftyply << '\n';

	stream << "rights: ";
	if (W_OO & p.rights)
		stream << 'K';
	if (W_OOO & p.rights)
		stream << 'Q';
	if (B_OO & p.rights)
		stream << 'k';
	if (B_OOO & p.rights)
		stream << 'q';
	stream << '\n';

	int index { lsb(p.enpassantsq) };

	stream << "en pas: ";
	if (index != -1)
	{
		stream << ('a' + index % BOARD_SIZE);
		stream << ('0' + index / BOARD_SIZE);
	}

	stream << '\n';

	return stream.str();
}

static const char* skipfirstwhite(const char* c)
{
	while (*c == ' ')
		c++;

	return c;
}

// Skips until after next whitespace
static const char* skipwhite(const char* c)
{
	while (*c != ' ' && *c != '\0')
		c++;

	while (*c == ' ')
		c++;

	return c;
}

Position load_fen(const char* fen)
{
	const char* c = skipfirstwhite(fen);
	Position    result;

	// Board
	{
		int x { 0 }, y { 7 };
		while (*c != ' ')
		{
			(isdigit(*c) || islower(*c) || isupper(*c) || *c == '/');

			if (isdigit(*c))
			{	 // Skip empty pieces
				x += *c - '0';
			}
			else if (*c == '/')
			{	 // Next
				x = 0;
				y--;
			}
			else
			{
				Piece p { isupper(*c) ? WP : BP };
				switch (toupper(*c))
				{
					case 'P':	 // Do nothing
						break;
					case 'N':
						p = (Piece)(p + 1);
						break;
					case 'B':
						p = (Piece)(p + 2);
						break;
					case 'R':
						p = (Piece)(p + 3);
						break;
					case 'Q':
						p = (Piece)(p + 4);
						break;
					case 'K':
						p = (Piece)(p + 5);
						break;
					default:
						break;
				}

				result.set(idx(x, y), p);
				x++;
			}

			c++;
		}
	}

	{	 // Color
		c = skipwhite(c);

		if (*c == 'w')
			result.side = WHITE;
		else if (*c == 'b')
			result.side = BLACK;
		else
			throw_assert(false, "Invald side in fen");
	}

	{	 // Castling rights
		c = skipwhite(c);

		if (*c == '-')
			goto enpassantsq;

		while (*c != ' ')
		{
			switch (*c)
			{
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
					throw_assert(false, "Invalid castling rights");
			}

			c++;
		}
	}

	{
	enpassantsq:
		c = skipwhite(c);

		if (*c == '-')
			goto fiftymoverule;

		int x { *c - 'a' };
		c++;
		int y { *c - '1' };

		throw_assert(is_inside(x, y), "En Passant Square");

		result.enpassantsq = cell(idx(x, y));
	}

	{
	fiftymoverule:
		c = skipwhite(c);

		result.fiftyply = atoi(c);
	}

	{	 // Hisply
		c = skipwhite(c);

		result.hisply = atoi(c) * 2 + (result.side == WHITE ? 0 : 1) - 2;

		throw_assert(result.hisply >= 0, "Invalid hisply");
	}

	return result;
}

std::string to_string(bitboard board)
{
	std::string res { "" };
	res.reserve(BOARD_SIZE * BOARD_SIZE + BOARD_SIZE);

	for (int y { BOARD_SIZE - 1 }; y >= 0; y--)
	{
		for (int x { 0 }; x < BOARD_SIZE; x++)
		{
			res += '0' + is_set(board, idx(x, y));
		}
		res += '\n';
	}

	return res;
}

static std::string field(Square sq)
{
	std::string field { "  " };
	field[0] = 'a' + file(sq);
	field[1] = '1' + rank(sq);
	return field;
}

std::string to_string(Move move)
{
	std::string readable { "" };
	readable.reserve(6);

	readable += PIECES[piece_type(move.piece())];
	readable += field(move.from());

	if (move.capture() != NO_PIECE)
	{
		readable += 'x';
		readable += PIECES[piece_type(move.capture())];
	}

	readable += field(move.to());

	return readable;
}

// Uci protocol

std::string to_uci(Move move)
{
	if (move.promotion() != NO_PIECE)
	{
		assert((char)tolower(PIECES[move.promotion()] != 'p'));
		return field(move.from()) + field(move.to()) + (char)tolower(PIECES[move.promotion()]);
	}

	return field(move.from()) + field(move.to());
}

Square field(char collumn, char row) { return idx(collumn - 'a', row - '1'); }

PieceType to_piece_type(char piece_type)
{
	piece_type = tolower(piece_type);
	std::cout << (piece_type) << std::endl;
	switch (piece_type)
	{
		case 'p':
			return PAWN;
		case 'n':
			return KNIGHT;
		case 'b':
			return BISHOP;
		case 'r':
			return ROOK;
		case 'q':
			return QUEEN;
		case 'k':
			return KING;
		default:
			break;
	}
	return NO_PIECE_TYPE;
}

Move from_uci(std::string move, const Position& pos)
{
	assert(move.size() >= 4);
	Square from { field(move[0], move[1]) };
	Square to { field(move[2], move[3]) };
	Piece  promotion { NO_PIECE };

	if (move.size() > 4)
	{
		promotion = combine(pos.side, to_piece_type(move[4]));
	}

	Move result;
	result.setFrom(from);
	result.setTo(to);
	result.setPromotion(promotion);
	Piece piece { pos.get(from) };
	result.setPiece(piece);
	Piece capture { pos.get(to) };
	result.setCapture(capture);
	PieceType pt { piece_type(piece) };
	result.flagEnPassant(pt == PAWN && pos.enpassantsq & cell(to) && capture != NO_PIECE);
	result.flagDoublePawnPush(pt == PAWN && abs(to - from) == 16);
	if (pt == KING && abs(from - to) == 2)
		result.setCastling((CastlingSide)(
			(pos.side == WHITE ? CS_WHITE : CS_BLACK) & (cell(to) & FILE_G ? KING_SIDE : QUEEN_SIDE)));

	return result;
}