

#include "init.h"

#include <iostream>
#include <chrono>

#include "hash.h"
#include "attacks.h"
#include "pv.h"

struct Timer
{
	std::chrono::high_resolution_clock::time_point start;
	Timer() { start = std::chrono::high_resolution_clock::now(); }

	size_t stop()
	{
		auto end { std::chrono::high_resolution_clock::now() };
		auto duration { end - start };
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	}
};

void init()
{
	Timer tim;

	size_t cache { 0 };

	cache += init_hash();
	cache += init_attacks();
	cache += init_pv(0xFFFFF);

	std::cout << "Cache size: " << (cache / 1024) << "KB\n";
	std::cout << "Initialization duration: " << tim.stop() << "ms" << std::endl;
}