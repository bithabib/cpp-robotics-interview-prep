cmake_minimum_required(VERSION 3.16)
project(astar_grid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Library: the planner itself. PUBLIC include dir so consumers get it too.
add_library(astar src/astar.cpp)
target_include_directories(astar PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(astar PRIVATE -Wall -Wextra -Wpedantic)

# Demo: astar_demo [maps/sample.txt] [4|8]
add_executable(astar_demo src/demo.cpp)
target_link_libraries(astar_demo PRIVATE astar)
target_compile_options(astar_demo PRIVATE -Wall -Wextra -Wpedantic)

# Tests. libgtest-dev on Ubuntu 22.04 ships GTestConfig.cmake, so find_package
# gives us the GTest::gtest_main imported target. Fallback: build from source.
enable_testing()
find_package(GTest QUIET)
if(NOT GTest_FOUND AND EXISTS /usr/src/googletest)
  add_subdirectory(/usr/src/googletest ${CMAKE_BINARY_DIR}/googletest EXCLUDE_FROM_ALL)
  add_library(GTest::gtest_main ALIAS gtest_main)
elseif(NOT GTest_FOUND)
  message(FATAL_ERROR "GoogleTest not found: apt install libgtest-dev")
endif()

add_executable(astar_test tests/astar_test.cpp)
target_link_libraries(astar_test PRIVATE astar GTest::gtest_main)
target_compile_options(astar_test PRIVATE -Wall -Wextra)
add_test(NAME astar_test COMMAND astar_test)
