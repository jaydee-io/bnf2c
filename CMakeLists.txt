cmake_minimum_required(VERSION 3.2)

project(bnf2c)

set(BNF2C_PATH "${CMAKE_SOURCE_DIR}")
set(BNF2C_CMAKE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
set(BNF2C_BUILD_PATH "${CMAKE_BINARY_DIR}")

list(APPEND CMAKE_MODULE_PATH "${BNF2C_CMAKE_PATH}")

add_subdirectory(src)

# Tests
enable_testing() # Need to be in top level CMakeLists.txt
add_subdirectory(utils/unittest/googletest)
add_subdirectory(test)
