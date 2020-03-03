
#include "rand64.h"

bitboard rand64() {
	return (
            static_cast<U64>(std::rand()) +
            (static_cast<U64>(std::rand()) << 15) +
            (static_cast<U64>(std::rand()) << 30) +
            (static_cast<U64>(std::rand()) << 45) +
            (static_cast<U64>(std::rand() & 0xf) << 60)
        );
}

bitboard rand64_fewbits() {
	return rand64() & rand64() & rand64();
}