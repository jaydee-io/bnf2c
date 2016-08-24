////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "Errors.h"

#include <iomanip>


#define COLOR_GREEN  "\033[0;32"
#define COLOR_RED    "\033[0;31"
#define COLOR_RESET  "\033[0"
#define COLOR_NORMAL "m"
#define COLOR_BOLD   ";1m"


////////////////////////////////////////////////////////////////////////////////
ParsingError::ParsingError(const LexerState & lexerState, unsigned long tokenLength, std::string && message)
: lineString(lexerState.getCurrentLine()), line(lexerState.getLine()), column(lexerState.getColumn()), nTabs(lexerState.getTabulations()), tokenLength(tokenLength), message(std::forward<std::string>(message))
{ }

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const CommandLineParsingError & error)
{
    if(!error.message.empty())
        os << error.message << std::endl;

    return os;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const ParsingError & error)
{
    os << COLOR_RED COLOR_BOLD "Parsing error" COLOR_RESET COLOR_BOLD " at L" << error.line << ":C" << error.column << " : " << error.message << COLOR_RESET COLOR_NORMAL << std::endl;
    os << error.lineString << std::endl;
    for(int i=0; i<error.nTabs; i++)
        os << '\t';
    os << COLOR_GREEN COLOR_BOLD << std::setw(error.column - error.nTabs) << std::setfill(' ') << std::right << '^';
    if(error.tokenLength > 1)
        os << std::setw(error.tokenLength - 1) << std::setfill('-') << std::right << '^' << std::setfill(' ') << COLOR_RESET COLOR_NORMAL;
    os << std::endl;

    return os;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const GeneratingError & error)
{
    os << COLOR_RED COLOR_BOLD "Generating error" COLOR_RESET COLOR_BOLD << " : " << error.message <<  COLOR_RESET COLOR_NORMAL << std::endl;

    return os;
}
