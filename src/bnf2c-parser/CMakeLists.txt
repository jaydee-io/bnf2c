################################################################################
#                                     BNF2C
#
# This file is distributed under the 4-clause Berkeley Software Distribution
# License. See LICENSE for details.
################################################################################
find_package(RE2C REQUIRED)

add_lexer(LexerBNF.re2c.cpp)

set(SOURCES
    Token.cpp
    LexerBNF.cpp
    ParserBNF.cpp
)

add_library(bnf2c-parser STATIC ${SOURCES})

