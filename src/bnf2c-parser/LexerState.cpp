////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "LexerState.h"

LexerState::LexerState(const char * input)
: input(input), lastNewLine(input), line(1), tabs(0)
{ }

int LexerState::getLine(void) const
{
    return line;
}

int LexerState::getColumn(void) const
{
    return input - lastNewLine + 1;
}

int LexerState::getTabulations(void) const
{
    return tabs;
}

std::string LexerState::getCurrentLine(void) const
{
    std::string currentLine;
    const char * endLine = lastNewLine;

    while((endLine[0] != '\0') && (endLine[0] != '\n') && (endLine[0] != '\r'))
        endLine++;

    currentLine.assign(lastNewLine, endLine - lastNewLine);

    return currentLine;
}

void LexerState::addNewline(const char * start)
{
    line++;
    lastNewLine = (start == nullptr) ? input : start;
    tabs = 0;
}

void LexerState::addTabulation(void)
{
    tabs++;
}
