project(Lizard-tests)

file(GLOB TESTS
    "*.h"
    "*.cpp"
)

include_directories(
	"../include"
)

list(REMOVE_ITEM SRC_TESTED ../src/main.cpp)

set(CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME} ${SRC_TESTED} ${TESTS})
target_link_libraries(${PROJECT_NAME} pthread)
target_link_libraries(${PROJECT_NAME} Lizard-lib)
