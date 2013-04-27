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
#include "LexerBNF.h"

#include <fstream>
#include <iostream>

/*!re2c
   re2c:define:YYCTYPE = "unsigned char";
   re2c:define:YYCURSOR = m_state.input;
   re2c:define:YYMARKER = marker;
   re2c:indent:top = 1;
   re2c:indent:string = "    ";
   re2c:yyfill:enable = 0;
*/

const std::string LexerBNF::BNF2C_TOKEN("/*!bnf2c");

////////////////////////////////////////////////////////////////////////////////
LexerBNF::LexerBNF(const char * input)
: m_state({input, input, 1, 0})
{
}

////////////////////////////////////////////////////////////////////////////////
bool LexerBNF::nextBnf2c(std::string & source)
{
    const char * start = m_state.input;
    source.clear();

    while((m_state.input[0] != '\0') && (LexerBNF::BNF2C_TOKEN.compare(0, std::string::npos, m_state.input, LexerBNF::BNF2C_TOKEN.size()) != 0))
    {
        if(m_state.input[0] == '\n')
        {
            if(m_state.input[1] == '\r')
                m_state.input++;

            m_state.lastNewLine = m_state.input + 1;
            m_state.line++;
        }
        else if(m_state.input[0] == '\r')
        {
            if(m_state.input[1] == '\n')
                m_state.input++;

            m_state.lastNewLine = m_state.input + 1;
            m_state.line++;
        }

        m_state.input++;
    }

    source.assign(start, m_state.input - start);
    m_state.input += LexerBNF::BNF2C_TOKEN.size();

    return (m_state.input[0] != '\0');
}

////////////////////////////////////////////////////////////////////////////////
void LexerBNF::nextToken(Token & token)
{
    const char * marker;

    for (;;)
    {
        token.value  = m_state.input;
        token.line   = m_state.line;
        token.column = getColumn();

        /*!re2c
        identifier = [a-zA-Z0-9_-]+;
        paramName  = [a-zA-Z0-9_-:]+;

        [ ]+  { continue; }
        [\t]+ { m_state.tabs++; continue; }

        "\n\r"|"\r\n"|"\n"|"\r" { token.type = TokenType::NEW_LINE; m_state.line++; m_state.lastNewLine = m_state.input; m_state.tabs = 0; break; }
        "::="                   { token.type = TokenType::AFFECTATION; break; }
        "|"                     { token.type = TokenType::OR;          break; }
        "{"                     { token.type = TokenType::BRACE_OPEN;  break; }
        "}"                     { token.type = TokenType::BRACE_CLOSE; break; }
        "="                     { token.type = TokenType::EQUAL;       break; }

        "<"identifier">"  { token.type = TokenType::INTERMEDIATE; break; }
        identifier        { token.type = TokenType::TERMINAL  ;   break; }
        "#"[^\n]+         { token.type = TokenType::COMMENT;      break; }
        "bnf2c:"paramName { token.type = TokenType::PARAM_NAME;   break; }
        '"'[^"]*'"'       { token.type = TokenType::PARAM_VALUE;  break; }

        "*\/"  {            token.type = TokenType::END_OF_INPUT; break; }
        "\000" { m_state.input--; token.type = TokenType::END_OF_INPUT; break; }
        [^]    { m_state.input--; token.type = TokenType::ERROR;        break; }
        */
    }

    token.end = m_state.input;
}

////////////////////////////////////////////////////////////////////////////////
bool LexerBNF::readRuleAction(std::string & ruleAction)
{
    const char * start = m_state.input;
    int nBraces = 1;

    while((nBraces > 0) && (m_state.input[0] != '\0'))
    {
        if(m_state.input[0] == '{')
            nBraces++;
        else if(m_state.input[0] == '}')
            nBraces--;
        else if(m_state.input[0] == '\n')
        {
            if(m_state.input[1] == '\r')
                m_state.input++;

            m_state.lastNewLine = m_state.input + 1;
            m_state.line++;
        }
        else if(m_state.input[0] == '\r')
        {
            if(m_state.input[1] == '\n')
                m_state.input++;

            m_state.lastNewLine = m_state.input + 1;
            m_state.line++;
        }

        m_state.input++;
    }

    if(nBraces == 0)
    {
        m_state.input--;
        ruleAction.assign(start, m_state.input - start);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
int LexerBNF::getLine(void) const
{
    return m_state.line;
}

////////////////////////////////////////////////////////////////////////////////
int LexerBNF::getColumn(void) const
{
    return m_state.input - m_state.lastNewLine + 1;
}

////////////////////////////////////////////////////////////////////////////////
int LexerBNF::getTabulations(void) const
{
    return m_state.tabs;
}

////////////////////////////////////////////////////////////////////////////////
std::string LexerBNF::getCurrentLine(void) const
{
    std::string currentLine;
    const char * endLine = m_state.lastNewLine;

    while((endLine[0] != '\0') && (endLine[0] != '\n') && (endLine[0] != '\r'))
        endLine++;

    currentLine.assign(m_state.lastNewLine, endLine - m_state.lastNewLine);

    return currentLine;
}

////////////////////////////////////////////////////////////////////////////////
const LexerBNF::State & LexerBNF::saveState(void) const
{
    return m_state;
}

////////////////////////////////////////////////////////////////////////////////
void LexerBNF::restoreState(const LexerBNF::State & state)
{
    m_state = state;
}
