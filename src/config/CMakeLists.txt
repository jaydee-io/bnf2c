################################################################################
#                                     BNF2C
#
# This file is distributed under the 4-clause Berkeley Software Distribution
# License. See LICENSE for details.
################################################################################

set(SOURCES
    ParameterizedString.cpp
    Options.cpp
)

add_library(bnf2c-config STATIC ${SOURCES})


