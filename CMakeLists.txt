cmake_minimum_required(VERSION 3.2)

project(rank_in_range)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

add_subdirectory(deps/googletest)

include_directories(include)
include_directories(deps/googletest/include)

add_executable(test include/rank_in_range/rank_in_range.h tests/test_rank_in_range.cpp)
target_link_libraries(test gtest_main pthread)
