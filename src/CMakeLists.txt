################################################################################
#                                     BNF2C
#
# This file is distributed under the 4-clause Berkeley Software Distribution
# License. See LICENSE for details.
################################################################################

set(SOURCES
    Streams.cpp
    Errors.cpp
    main.cpp
)

add_executable(bnf2c ${SOURCES})

target_link_libraries(bnf2c
    bnf2c-parser
    bnf2c-generator
    bnf2c-core
    bnf2c-config
    bnf2c-printer
)

install(TARGETS bnf2c DESTINATION bin)
