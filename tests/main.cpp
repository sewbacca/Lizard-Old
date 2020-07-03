#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "init.h"

int main(int argc, char* argv[])
{
	init();

	int result { Catch::Session().run(argc, argv) };

	return result;
}