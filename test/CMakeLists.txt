find_package(BNF2C REQUIRED)
include(AddUnitTest)

add_lexer (calc.re2c.bnf2c.cpp)
add_parser(calc.bnf2c.cpp)

add_lexer (wikipedia.re2c.bnf2c.c)
add_parser(wikipedia.bnf2c.c)

set_source_files_properties(wikipedia.c PROPERTIES COMPILE_FLAGS -std=c90)

add_library_unittest(Bnf2cTests
    calc.cpp
    wikipedia.c
    wikipedia_main.cpp
)

