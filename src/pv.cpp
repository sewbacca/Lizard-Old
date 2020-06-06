
#include "pv.h"

struct Move_pv {
	U64 hash = 0;
	Move move = NO_MOVE;
};

Move_pv* pv_table = nullptr;
size_t size = 0;


size_t init_pv(size_t bytes) {
	size = bytes / sizeof(Move_pv);
	pv_table = new Move_pv[size];

	return bytes;
}

void clear_pv() {
	for(size_t i = 0; i < size; i++) {
		pv_table[i] = Move_pv();
	}
}

Move probe_pv(U64 hash) {
	Move_pv& pv = pv_table[hash % size];

	if(pv.hash != hash) {
		return NO_MOVE;
	}

	return pv.move;
}

void store_pv(U64 hash, Move move) {
	pv_table[hash % size] = Move_pv{ hash, move };
}