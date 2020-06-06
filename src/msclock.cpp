
#include <chrono>
#include "msclock.h"

uint64_t msclock() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
}
