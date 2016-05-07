find_package(RE2C REQUIRED)

add_lexer(LexerBNF.re2c.cpp)

set(SOURCES
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

add_executable(bnf2c ${SOURCES})
