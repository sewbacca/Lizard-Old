
cmake_minimum_required(VERSION 3.20.5)

project(Lizard)

set(CMAKE_CXX_STANDARD 17)
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME}
	src/main.cpp
)

add_library(${PROJECT_NAME}-engine
	src/attacks.cpp
	src/bitboard_constants.cpp
	src/bitboard.cpp
	src/def.cpp
	src/enum_operator.cpp
	src/eval.cpp
	src/gamephase.cpp
	src/hash.cpp
	src/init.cpp
	src/magic.cpp
	src/movegen.cpp
	src/msclock.cpp
	src/perft.cpp
	src/position.cpp
	src/pv.cpp
	src/rand64.cpp
	src/search.cpp
	src/textutils.cpp
	src/uci.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC src)
target_include_directories(${PROJECT_NAME} PUBLIC src)

target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}-engine)
