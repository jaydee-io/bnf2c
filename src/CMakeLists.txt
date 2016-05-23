################################################################################
#                                     BNF2C
#
# This file is distributed under the 4-clause Berkeley Software Distribution
# License. See LICENSE for details.
################################################################################
find_package(RE2C REQUIRED)

add_lexer(LexerBNF.re2c.cpp)

set(SOURCES
    generator/ParserGenerator.cpp
    generator/StateGenerator.cpp
    generator/SwitchGenerator.cpp
	Token.cpp
	LexerBNF.cpp
	ParserBNF.cpp
	Dictionary.cpp
	Symbol.cpp
	Rule.cpp
	Grammar.cpp
	ParserState.cpp
	ParseTable.cpp
	Options.cpp
	Errors.cpp
	main.cpp
)

add_compile_options(
    -std=c++14
    -Werror
)

add_executable(bnf2c ${SOURCES})

install(TARGETS bnf2c DESTINATION bin)
