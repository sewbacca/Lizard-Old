
#include <iostream>
#include "init.h"
#include "uci.h"

int main(int argc, char const *argv[])
{
	init();
	uci_loop("Sewbacca", "Lizard");
}
