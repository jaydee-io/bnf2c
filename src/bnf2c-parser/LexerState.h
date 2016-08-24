////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _LEXER_STATE_H_
#define _LEXER_STATE_H_
#include <string>

struct LexerState
{
    const char * input;
    const char * lastNewLine;
    int          line;
    int          tabs;

    LexerState(const char * input);

    // Line & Column start at 1
    int getLine(void) const;
    int getColumn(void) const;

    // Number of tabulations in the current line
    int getTabulations(void) const;

    std::string getCurrentLine(void) const;

    void addNewline(const char * start = nullptr);
    void addTabulation(void);
};

#endif /* _LEXER_STATE_H_ */

