
#include <iostream>
using std::cout, std::cin, std::cerr, std::endl;
#include <sstream>
using std::getline, std::string, std::stringstream;
#include <thread>
using std::thread;
#include <functional>
using std::function;
#include <map>
using std::map;

#include "uci.h"
#include "textutils.h"
#include "perft.h"

#include "position.h"
#include "search.h"
#include "attacks.h"
#include "except.h"
#include "msclock.h"

static void assertmove(Move move, Position& pos)
{
	pos.make_move(move);
	if (is_in_check(pos, swap(pos.side)))
	{
		pos.undo_move();
		throw_assert(false, "Invalid move");
	}
}

class Uci
{
// Hookup

private:
	map<string, function<void(stringstream&&)>> commands {};
public:
	Uci(string author, string engine_name);
	void main();

// Command api

private: 
	static void log(string message) { cerr << "info string " << message << std::endl; };
	static void print(string message) { cout << message << std::endl; };
	static void msgerror(string message) { cerr << message << std::endl; }

// Commands

private:
	bool running = true;
	Position pos {};
	thread engine {};
	thread watcher {};
	SearchInfo info {};
	string author;
	string engine_name;
private:
	void uci(stringstream&&);
	void isready(stringstream&&);
	void quit(stringstream&&);
	void ucinewgame(stringstream&&);
	void read_position(stringstream&&);
	void printboard(stringstream&&);
	void perft_divide(stringstream&&);
	void perft_timed(stringstream&&);
	void go(stringstream&&);
	void stop(stringstream&&);
};

Uci::Uci(string author, string engine_name) : author { author }, engine_name { engine_name }
{
	// Registering commands

	#define REGISTER_AS(name, uci_name) commands[uci_name] = std::bind(&Uci::name, this, std::placeholders::_1)
	#define REGISTER(name) REGISTER_AS(name, #name)
	
	REGISTER(uci);
	REGISTER(quit);
	REGISTER_AS(quit, "exit");
	REGISTER(isready);
	REGISTER(printboard);
	REGISTER_AS(printboard, "d");
	REGISTER(ucinewgame);
	REGISTER_AS(read_position, "position");
	REGISTER_AS(perft_divide, "perft");
	REGISTER_AS(perft_timed, "perft-timed");
	REGISTER(go);
	REGISTER(stop);

	#undef REGISTER
	#undef REGISTER_AS
}

void Uci::main()
{
	while(running)
	{
		string line;
		if (!cin)
			throw std::runtime_error("Bad cin");
		getline(cin, line);
		stringstream stream(line);

		string cmd;

		stream >> cmd;

		if(cmd == "")
			continue;
		
		if(commands.find(cmd) == commands.end())
		{
			log("Command '" + cmd + "'not found!");
			continue;
		}

		commands[cmd](std::move(stream));
	}
}

// Commands

void Uci::uci(stringstream&&)
{
	print("id author " + author);
	print("id name " + engine_name);
	print("uciok");
}

void Uci::isready(stringstream&&)
{
	print("uciok");
}

void Uci::ucinewgame(stringstream&&)
{
	pos = load_fen(startpos);
}

void Uci::read_position(stringstream&& line)
{
	// Kill thread first
	info.stopped = true;
	if (engine.joinable())
		engine.join();

	string fen { "" };
	string item { "" };

	line >> item;
	if (item == "startpos")
	{
		line >> item;
		fen = startpos;
		goto loadfen;
	}

	do
	{
		line >> item;
		if (item == "moves")
			break;
		fen += item + ' ';
	} while (line);

loadfen:
	try
	{
		pos = load_fen(fen.c_str());

		while (line)
		{
			line >> item;
			assertmove(from_uci(item, pos), pos);
		}
	}
	catch (std::exception& e)
	{
		msgerror(string("Invalid fen: ") + e.what());
	}

}

void Uci::printboard(stringstream&&)
{
	print(to_string(pos));
}

void Uci::perft_divide(stringstream&& line)
{
	size_t depth;
	line >> depth;

	perft(depth, pos);
}

void Uci::perft_timed(stringstream&& line)
{
	size_t depth;
	line >> depth;

	size_t ms { msclock() };
	perft(depth, pos);
	std::cout << msclock() - ms << "ms" << std::endl;
}

void Uci::go(stringstream&& line)
{
	// Kill thread first
	info.stopped = true;
	if (engine.joinable())
		engine.join();

	info = SearchInfo {};

	int time_inc { 0 };
	int time_left { -1 };
	int movetime { -1 };
	int depth { MAX_DEPTH };
	int movestogo { 30 };

	while (!line.eof())
	{
		string name;
		int    value;

		line >> name;

		if (name == "wtime" && pos.side == WHITE)
		{
			line >> value;
			time_left = value;
		}
		else if (name == "btime" && pos.side == BLACK)
		{
			line >> value;
			time_left = value;
		}
		else if (name == "winc" && pos.side == WHITE)
		{
			line >> value;
			time_inc = value;
		}
		else if (name == "binc" && pos.side == BLACK)
		{
			line >> value;
			time_inc = value;
		}
		else if (name == "depth")
		{
			line >> value;
			depth	      = value;
			info.infinite = true;
		}
		else if (name == "movestogo")
		{
			line >> value;
			movestogo = value;
		}
		else if (name == "movetime")
		{
			line >> value;
			movetime = value;
		}
	}

	if (movetime == -1)
	{
		movetime = time_left / movestogo + time_inc - 50;	 // 50 ms buffer
	}

	info.depth   = depth;
	info.end     = msclock() + movetime;
	info.stopped = false;

	engine = thread(search, &info, &pos, [this](const ReportInfo& rinfo) {
		if (rinfo.finished)
		{
			std::cout << to_string(pos);
			std::cout << "bestmove " << to_uci(rinfo.bestmove);
		}
		else
		{
			std::cout << "info depth " << rinfo.depth << " nodes " << rinfo.nodes
					<< " time " << rinfo.time << " nodes " << rinfo.nodes << " score "
					<< rinfo.score_type << " " << rinfo.score;

			std::cout << " pv";

			for (Move move : rinfo.pv)
			{
				std::cout << " " << to_uci(move);
			}
		}
		std::cout << std::endl;
	});
}

void Uci::quit(stringstream&& line)
{
	running = false;
}

void Uci::stop(stringstream&& line)
{
	info.stopped = true;
	if (engine.joinable())
		engine.join();
}

// Main entry point

void uci_main(string author, string engine_name)
{
	Uci(author, engine_name).main();
}