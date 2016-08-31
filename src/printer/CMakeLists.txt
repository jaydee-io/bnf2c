################################################################################
#                                     BNF2C
#
# This file is distributed under the 4-clause Berkeley Software Distribution
# License. See LICENSE for details.
################################################################################
find_package(RE2C REQUIRED)

set(SOURCES
    SymbolPrinter.cpp
    ItemPrinter.cpp
    RulePrinter.cpp
    GrammarPrinter.cpp
    ParserPrinter.cpp
    ErrorsPrinter.cpp
    TokenPrinter.cpp
    OptionsPrinter.cpp
)

add_library(bnf2c-printer STATIC ${SOURCES})

