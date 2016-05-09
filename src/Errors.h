////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef __ERRORS_H__
#define __ERRORS_H__
#include <string>
#include <list>

#include "LexerBNF.h"
#include "Token.h"

// Command line parsing error
struct CommandLineParsingError
{
    std::string message;
};

// Parsing error
struct ParsingError // : public Error
{
    int         line;
    int         column;
    int         nbTabs;
    std::string currentLine;
    std::string value;

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

#endif /* __ERRORS_H__ */
