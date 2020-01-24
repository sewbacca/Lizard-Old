#include "def.h"

bool is_inside(int x, int y) {
	return x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE;
}