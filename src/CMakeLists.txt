
project(Lizard-lib)

file(GLOB SRC_ELSE
    "*.h"
    "*.cpp"
)

set(CMAKE_CXX_STANDARD 17)
list(REMOVE_ITEM SRC_ELSE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
add_library(${PROJECT_NAME} STATIC ${SRC_ELSE})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

project(Lizard)

set(CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}-lib)
target_link_libraries(${PROJECT_NAME} -static -lpthread)
