################################################################################
#                                     BNF2C
#
# This file is distributed under the 4-clause Berkeley Software Distribution
# License. See LICENSE for details.
################################################################################
find_package(RE2C REQUIRED)

set(SOURCES
    Symbol.cpp
    Rule.cpp
    Item.cpp
    Grammar.cpp
    ParsingAction.cpp
    ParserState.cpp
    Parser.cpp
    LR0/LR0Parser.cpp
    LR0/LR0State.cpp
    LR1/LR1Parser.cpp
    LR1/LR1State.cpp
    LALR1/LALR1Parser.cpp
    LALR1/LALR1State.cpp
)

add_library(bnf2c-core STATIC ${SOURCES})

