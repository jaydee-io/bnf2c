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
#ifndef __PARSERBNF_H__
#define __PARSERBNF_H__
#include "LexerBNF.h"
#include "Grammar.h"
#include "Options.h"
#include "Rule.h"

#include <string>
#include <list>
#include <iostream>
#include <unordered_map>

struct ParsingError
{
    const LexerBNF &  lexer;
    const Token &     token;
    const std::string message;
};

std::ostream & operator <<(std::ostream & os, const ParsingError & error);

class ParserBNF
{
    protected :
        typedef std::unordered_map<std::string, std::string *>  StringParamMap;
        typedef std::unordered_map<std::string, bool *>         BoolParamMap;
        typedef std::unordered_map<std::string, unsigned int *> UintParamMap;

    public :
        ParserBNF(LexerBNF & lexer, Options & options, Grammar & grammar);

        void parseBnf2cBlock(void) throw(ParsingError);

    protected :
        void parseRule(Rule & rule) throw(ParsingError);
        void parseParameter(void) throw(ParsingError);

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
