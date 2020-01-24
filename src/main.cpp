
#include <iostream>
#include "engine/textutils.h"

int main(int argc, char const *argv[])
{
    std::cout << to_string(load_fen(startpos));
    return 0;
}
