////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _ERRORS_H_
#define _ERRORS_H_
#include <string>
#include <list>

#include "bnf2c-parser/LexerState.h"

// Command line parsing error
struct CommandLineParsingError
{
    std::string message;
};

// Parsing error
struct ParsingError // : public Error
{
    ParsingError(const LexerState & lexerState, unsigned long tokenLength, std::string && message);

    std::string   lineString;
    int           line;
    int           column;
    int           nTabs;
    unsigned long tokenLength;

    std::string message;
};

// Generating error
struct GeneratingError // : public Error
{
    const std::string message;
};

std::ostream & operator <<(std::ostream & os, const CommandLineParsingError & error);
std::ostream & operator <<(std::ostream & os, const ParsingError & error);
std::ostream & operator <<(std::ostream & os, const GeneratingError & error);

// Errors list
template<class ErrorType>
struct Errors
{
    std::list<ErrorType> list;
    int                  exitCode;
};

template<class ErrorType>
std::ostream & operator <<(std::ostream & os, const Errors<ErrorType> & errors)
{
    for(const ErrorType & error : errors.list)
        os << error;

    return os;
}

#endif /* _ERRORS_H_ */
