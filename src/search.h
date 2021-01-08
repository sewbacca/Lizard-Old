
#pragma once

#include <cstddef>
#include <functional>

#include "position.h"

constexpr int MATE { 30000 };
constexpr int INFINITE { 31000 };
constexpr int MAX_DEPTH { 64 };

struct SearchInfo
{
	size_t depth { MAX_DEPTH };
	U64    start;
	U64    end;
	size_t nodes;
	Move   bestmove;
	bool   stopped { false };
	bool   infinite { false };

	int fhf { 0 };
	int fh { 0 };
};

struct ReportInfo
{
	int		  depth {};
	int		  nodes {};
	U64		  time {};
	int		  score { 0 };
	std::string	  score_type { "cp" };
	std::vector<Move> pv {};
	Move		  bestmove { NO_MOVE };
	bool		  finished { false };
};

void search(SearchInfo* info, Position* pos, std::function<void(const ReportInfo& info)> report);