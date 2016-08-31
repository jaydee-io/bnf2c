################################################################################
#                                     BNF2C
#
# This file is distributed under the 4-clause Berkeley Software Distribution
# License. See LICENSE for details.
################################################################################
cmake_minimum_required(VERSION 3.2)

project(bnf2c)

set(BNF2C_PATH         "${CMAKE_SOURCE_DIR}")
set(BNF2C_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/src")
set(BNF2C_CMAKE_PATH   "${CMAKE_SOURCE_DIR}/cmake/modules")
set(BNF2C_BUILD_PATH   "${CMAKE_BINARY_DIR}")

list(APPEND CMAKE_MODULE_PATH "${BNF2C_CMAKE_PATH}")

include_directories(${BNF2C_INCLUDE_PATH})

# Tests
enable_testing() # Need to be in top level CMakeLists.txt
add_subdirectory(utils/unittest/googletest)
add_subdirectory(test)

# Global compil flags
add_compile_options(
    -std=c++14
    -Werror
)

# All bnf2c modules
add_subdirectory(src/bnf2c-parser)
add_subdirectory(src/core)
add_subdirectory(src/config)
add_subdirectory(src/generator)
add_subdirectory(src/printer)
add_subdirectory(src)
