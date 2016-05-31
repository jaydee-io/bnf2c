################################################################################
#                                     BNF2C
#
# This file is distributed under the 4-clause Berkeley Software Distribution
# License. See LICENSE for details.
################################################################################
find_package(RE2C REQUIRED)

set(SOURCES
    Dictionary.cpp
    Symbol.cpp
    Rule.cpp
    Grammar.cpp
    ParserState.cpp
    ParseTable.cpp
)

add_compile_options(
    -std=c++14
    -Werror
)

add_library(bnf2c-core STATIC ${SOURCES})

