////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef __PARSERBNF_H__
#define __PARSERBNF_H__
#include "LexerBNF.h"
#include "Grammar.h"
#include "Options.h"
#include "Rule.h"
#include "Errors.h"

#include <string>
#include <list>
#include <iostream>
#include <unordered_map>

class ParserBNF
{
    protected :
        typedef std::unordered_map<std::string, std::string *>  StringParamMap;
        typedef std::unordered_map<std::string, bool *>         BoolParamMap;
        typedef std::unordered_map<std::string, unsigned int *> UintParamMap;

    public :
        ParserBNF(LexerBNF & lexer, Options & options, Grammar & grammar);

        void parseBnf2cBlock(void);

        Errors<ParsingError> errors;

    protected :
        void parseRule(Rule & rule);
        void parseParameter(void);
        void parseIntermediatesTypes(void);

    protected :
        Grammar &       m_grammar;
        Options &       m_options;

        LexerBNF &      m_lexer;
        Token           m_token;

        StringParamMap  m_stringParams;
        BoolParamMap    m_boolParams;
        UintParamMap    m_uintParams;

        std::string     m_lastIntermediate;
};

#endif /* __PARSERBNF_H__ */
