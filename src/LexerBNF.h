////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef __LEXERBNF_H__
#define __LEXERBNF_H__
#include "Token.h"

#include <string>
#include <ostream>

class LexerBNF
{
    public :

        static const std::string BNF2C_TOKEN;

    public :
        LexerBNF(const std::string & input, std::ostream & output);

        bool moveToNextBnf2cBlock(void);
        void nextToken(Token & token);
        bool readRuleAction(std::string & ruleAction);

        int getLine(void) const;
        int getColumn(void) const;
        int getTabulations(void) const;

        std::string getCurrentLine(void) const;

    protected :
        const char * m_input;

        const char * m_lastNewLine;
        int          m_line;
        int          m_tabs;

        std::ostream &  m_output;
};

#endif /* __LEXERBNF_H__ */
