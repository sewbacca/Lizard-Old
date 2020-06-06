
#include <iostream>
#include <sstream>
#include <thread>

#include "uci.h"
#include "textutils.h"
#include "perft.h"

#include "position.h"
#include "search.h"
#include "msclock.h"

constexpr size_t MAX_LINE = 16384;
using namespace std;

void uci_loop(string author, string engine_name) {
	Position pos;
	thread engine;
	thread watcher;
	SearchInfo info;
	while(true) {
		char line_raw[MAX_LINE];
		cin.getline(line_raw, MAX_LINE);
		stringstream line(line_raw);

		string cmd;
		line >> cmd;

		if(cmd == "uci") {
			cout << "id author " << author << '\n';
			cout << "id name " << engine_name << '\n';
			cout << "uciok" << endl;
		}
		else if(cmd == "isready") cout << "readyok" << endl;
		else if(cmd == "ucinewgame") {
			pos = load_fen(startpos);
		} else if(cmd == "position") {
			string fen = "";
			string item = "";
			
			line >> item;
			if(item == "startpos") {
				line >> item;
				fen = startpos;
				goto loadfen;
			}

			do {
				line >> item;
				if(item == "moves") break;
				fen += item + ' ';
			} while(!line.eof());

			loadfen:
			pos = load_fen(fen.c_str());

			while(!line.eof()) {
				line >> item;
				pos.makeMove(from_uci(item, pos));
			}
		} else if(cmd == "d") {
			cout << to_string(pos) << endl;
		} else if(cmd == "perft") {
			size_t depth;
			line >> depth;
			perft_divide(depth, pos);
		} else if(cmd == "go") {
			// Kill thread first
			info.stopped = true;
			if(engine.joinable()) engine.join();

			int time_inc = 0;
			int time_left = -1;
			int movetime = -1;
			int depth = MAX_DEPTH;
			int movestogo = 30;

			while(!line.eof()) {
				string name;
				int value;

				line >> name;

				if (name == "wtime" && pos.side == WHITE) {
					line >> value;
					time_left = value;
				} else if(name == "btime" && pos.side == BLACK) {
					line >> value;
					time_left = value;
				} else if (name == "winc" && pos.side == WHITE) {
					line >> value;
					time_inc = value;
				} else if(name == "binc" && pos.side == BLACK) {
					line >> value;
					time_inc = value;
				} else if(name == "depth") {
					line >> value;
					depth = value;
					info.infinite = true;
				} else if(name == "movestogo") {
					line >> value;
					movestogo = value;
				} else if(name == "movetime") {
					line >> value;
					movetime = value;
				}
			}

			if (movetime == -1)
			{
				movetime = time_left / movestogo + time_inc - 50; // 50 ms buffer
			}

			info.depth = depth;
			info.end = msclock() + movetime;
			info.stopped = false;
			
			engine = thread(search, &info, &pos);
		} else if(cmd == "quit") {
			return;
		} else if(cmd == "stop") {
			info.stopped = true;
			if(engine.joinable()) engine.join();
		}
	}
}