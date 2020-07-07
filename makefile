all: exec Lizard test

ifndef VERBOSE
.SILENT:
endif

ifndef CONFIG
	CONFIG = Release
endif

# Windows
ifeq ($(OS), Windows_NT)
CXX = C:\MinGW\bin\g++.exe
CMAKE = "C:\Program Files\CMake/bin\cmake.exe"
MAKE = C:\MinGW\bin\make.exe

ifndef PLATFORM

PLATFORM = Windows
endif

# Linux
else
CMAKE = /usr/bin/cmake
MAKE = /usr/bin/make

ifndef PLATFORM
	PLATFORM = Linux
endif

ifeq ($(PLATFORM), Windows)
	CXX = /usr/bin/x86_64-w64-mingw32-g++-posix
endif

ifeq ($(PLATFORM), Linux)
	CXX = /usr/bin/g++
endif

endif

switch: clean-cache
	$(CMAKE) --no-warn-unused-cli -DCMAKE_BUILD_TYPE=$(CONFIG) -DCMAKE_CXX_COMPILER:FILEPATH=$(CXX) -G "Unix Makefiles" -B./build

exec:
	$(CMAKE) -S . -B ./build
	cd build && $(MAKE) Lizard

test:
	# $(CMAKE) --build ./build --target Lizard-tests
	$(CMAKE) -S . -B ./build
	cd build && $(MAKE) Lizard-tests

clean-bin:
	rm -rf bin/*

clean:
	cd build && $(MAKE) clean

clean-cache:
	rm -f build/CMakeCache.txt
	rm -rf build/CMakeFiles