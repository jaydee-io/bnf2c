////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013, Jerome DUMESNIL
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by Jerome DUMESNIL.
// 4. Neither the name of the Jerome DUMESNIL nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY JEROME DUMESNIL ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL JEROME DUMESNIL BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
        os << std::setw(error.value.size() - 1) << std::setfill('-') << std::right << '^' << COLOR_RESET COLOR_NORMAL << std::endl;
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
