////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _LEXERBNF_H_
#define _LEXERBNF_H_
#include "Token.h"
#include "LexerState.h"

#include <string>
#include <ostream>

class LexerBNF
{
    public :
        static const std::string BNF2C_TOKEN;

        LexerBNF(const std::string & input, std::ostream & output);

        bool moveToNextBnf2cBlock(void);

        Token nextToken(void);
        Token nextTokenSkippedComments(void);

        bool readRuleAction(std::string & ruleAction);

        const LexerState & getState(void);
        const LexerState & getLastState(void);

    protected :
        Token newToken(const TokenType type);

        LexerState m_state;
        LexerState m_lastState;

        std::ostream &  m_output;
};

#endif /* _LEXERBNF_H_ */
