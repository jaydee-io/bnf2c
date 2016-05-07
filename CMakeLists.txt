cmake_minimum_required(VERSION 3.2)

project(bnf2c)

set(BNF2C_PATH "${CMAKE_SOURCE_DIR}")
set(BNF2C_CMAKE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
set(BNF2C_BUILD_PATH "${CMAKE_BINARY_DIR}")

list(APPEND CMAKE_MODULE_PATH "${BNF2C_CMAKE_PATH}")

add_compile_options(
    -std=c++14
    -Werror
)

add_subdirectory(src)
