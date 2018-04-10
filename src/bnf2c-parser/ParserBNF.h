////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef PARSERBNF_H
#define PARSERBNF_H
#include "Token.h"
#include "config/Options.h"
#include "Errors.h"

#include <string>
#include <unordered_map>

class ParameterizedString;
class LexerBNF;
class Grammar;
class Rule;

class ParserBNF
{
    protected :
        typedef std::unordered_map<std::string, ParameterizedString *>  ParameterizedStringParamMap;
        typedef std::unordered_map<std::string, std::string *>  StringParamMap;
        typedef std::unordered_map<std::string, bool *>         BoolParamMap;
        typedef std::unordered_map<std::string, unsigned int *> UintParamMap;

    public :
        ParserBNF(LexerBNF & lexer, Grammar & grammar);

        void parseBnf2cBlock(void);

        Options & getInFileOptions(void) { return m_options; }

        Errors<ParsingError> errors;

    protected :
        void parseRule(Rule & rule);
        void parseParameter(void);
        void parseIntermediatesTypes(void);

    protected :
        Grammar &       m_grammar;
        Options         m_options;

        LexerBNF &      m_lexer;
        Token           m_token;

        ParameterizedStringParamMap  m_parameterizedStringParams;
        StringParamMap  m_stringParams;
        BoolParamMap    m_boolParams;
        UintParamMap    m_uintParams;

        std::string     m_lastIntermediate;
};

#endif /* PARSERBNF_H */
