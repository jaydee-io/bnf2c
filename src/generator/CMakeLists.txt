################################################################################
#                                     BNF2C
#
# This file is distributed under the 4-clause Berkeley Software Distribution
# License. See LICENSE for details.
################################################################################

set(SOURCES
    ParserGenerator.cpp
    StateGenerator.cpp
    SwitchGenerator.cpp
    FunctionGenerator.cpp
)

add_library(bnf2c-generator STATIC ${SOURCES})

