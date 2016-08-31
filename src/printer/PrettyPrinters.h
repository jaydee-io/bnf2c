////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _PRETTY_PRINTER_H_
#define _PRETTY_PRINTER_H_
#include <ostream>

// Core objects
class Symbol;
class Item;
class Rule;
class ParserState;
class Parser;
class Grammar;

std::ostream & operator <<(std::ostream & os, const Symbol & symbol);
std::ostream & operator <<(std::ostream & os, const Item & item);
std::ostream & operator <<(std::ostream & os, const Rule & rule);
std::ostream & operator <<(std::ostream & os, const Grammar & grammar);
std::ostream & operator <<(std::ostream & os, const ParserState & state);
std::ostream & operator <<(std::ostream & os, const Parser & parser);



// bnf2c parser
enum class TokenType;
class Token;
std::ostream & operator<<(std::ostream & os, const TokenType tokenType);
std::ostream & operator<<(std::ostream & os, const Token & token);



// Errors
class CommandLineParsingError;
class ParsingError;
class GeneratingError;
template<class ErrorType>
class Errors;

std::ostream & operator <<(std::ostream & os, const CommandLineParsingError & error);
std::ostream & operator <<(std::ostream & os, const ParsingError & error);
std::ostream & operator <<(std::ostream & os, const GeneratingError & error);

template<class ErrorType>
std::ostream & operator <<(std::ostream & os, const Errors<ErrorType> & errors)
{
    for(const ErrorType & error : errors.list)
        os << error;

    return os;
}



// Options
class Options;

std::ostream & operator <<(std::ostream & os, const Options & options);

#endif /* _PRETTY_PRINTER_H_ */
