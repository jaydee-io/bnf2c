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
std::ostream & operator <<(std::ostream & os, const CommandLineParsingError & error)
{
    if(!error.message.empty())
        os << error.message << std::endl;

    return os;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const ParsingError & error)
{
    std::string currentLine = error.currentLine;

    os << COLOR_RED COLOR_BOLD "Parsing error" COLOR_RESET COLOR_BOLD " at L" << error.line << ":C" << error.column << " : " << error.message << COLOR_RESET COLOR_NORMAL << std::endl;
    os << currentLine << std::endl;
    if(error.value.size() > 1)
    {
        for(int i=0; i<error.nbTabs; i++)
            os << '\t';
        os << COLOR_GREEN COLOR_BOLD << std::setw(error.column - error.nbTabs) << std::setfill(' ') << std::right << '^';
        os << std::setw(error.value.size() - 1) << std::setfill('-') << std::right << '^' << std::setfill(' ') << COLOR_RESET COLOR_NORMAL << std::endl;
    }
    else
    {
        for(int i=0; i<error.nbTabs; i++)
            os << '\t';
        os << COLOR_GREEN COLOR_BOLD << std::setw(error.column - error.nbTabs) << std::right << '^' << COLOR_RESET COLOR_NORMAL << std::endl;
    }

    return os;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const GeneratingError & error)
{
    os << COLOR_RED COLOR_BOLD "Generating error" COLOR_RESET COLOR_BOLD << " : " << error.message <<  COLOR_RESET COLOR_NORMAL << std::endl;

    return os;
}
