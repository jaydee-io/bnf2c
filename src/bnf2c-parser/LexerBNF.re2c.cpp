////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
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
LexerBNF::LexerBNF(const std::string & input, std::ostream & output)
: m_state(input.c_str()), m_lastState(input.c_str()), m_output(output)
{
}

////////////////////////////////////////////////////////////////////////////////
bool LexerBNF::moveToNextBnf2cBlock(void)
{
    const char * start = m_state.input;

    while((m_state.input[0] != '\0') && (LexerBNF::BNF2C_TOKEN.compare(0, std::string::npos, m_state.input, LexerBNF::BNF2C_TOKEN.size()) != 0))
    {
        if(m_state.input[0] == '\n')
        {
            if(m_state.input[1] == '\r')
                m_state.input++;

            m_state.addNewline(m_state.input + 1);
        }
        else if(m_state.input[0] == '\r')
        {
            if(m_state.input[1] == '\n')
                m_state.input++;

            m_state.addNewline(m_state.input + 1);
        }

        m_state.input++;
    }

    m_output.write(start, m_state.input - start);
    if(m_state.input[0] != '\0')
        m_state.input += LexerBNF::BNF2C_TOKEN.size();

    return (m_state.input[0] != '\0');
}

////////////////////////////////////////////////////////////////////////////////
Token LexerBNF::nextToken(void)
{
    const char * marker;

    for (;;)
    {
        m_lastState = m_state;

        /*!re2c
        identifier = [a-zA-Z0-9_\-]+;
        paramName  = [a-zA-Z0-9_\-:]+;
        typeName   = [a-zA-Z0-9_\-\.:<>]+;

        [ ]+                    { continue; }
        [\t]                    { m_state.addTabulation(); continue; }

        "\n\r"|"\r\n"|"\n"|"\r" { m_state.addNewline(); return newToken(TokenType::NEW_LINE); }

        "::="                    { return newToken(TokenType::AFFECTATION); }
        "|"                      { return newToken(TokenType::OR); }
        "{"                      { return newToken(TokenType::BRACE_OPEN); }
        "}"                      { return newToken(TokenType::BRACE_CLOSE); }
        "="                      { return newToken(TokenType::EQUAL); }

        "<"identifier">"         { return newToken(TokenType::INTERMEDIATE); }
        identifier               { return newToken(TokenType::TERMINAL); }
        "#"[^\n\r\000]*          { return newToken(TokenType::COMMENT); }
        "bnf2c:type<"typeName">" { return newToken(TokenType::TYPE_NAME); }
        "bnf2c:"paramName        { return newToken(TokenType::PARAM_NAME); }
        '"'("\\\""|[^"])*'"'     { return newToken(TokenType::PARAM_VALUE); }

        "*\/"  {                   return newToken(TokenType::END_OF_INPUT); }
        "\000" { m_state.input--;  return newToken(TokenType::END_OF_INPUT); }

        [^]    { break; }
        */
    }

    return newToken(TokenType::ERROR);
}

////////////////////////////////////////////////////////////////////////////////
bool LexerBNF::readRuleAction(std::string & ruleAction)
{
    int nBraces = 1;

    // Find start of action
    while((nBraces > 0) && (m_state.input[0] != '\0'))
    {
        // Skip spaces and tabulations
        if((m_state.input[0] != ' ') && (m_state.input[0] != '\t'))
            break;

        // Skip unix-style new line
        if(m_state.input[0] == '\n')
        {
            if(m_state.input[1] == '\r')
                m_state.input++;

            m_state.addNewline(m_state.input + 1);
            break;
        }
        // Skip windows-style new line
        else if(m_state.input[0] == '\r')
        {
            if(m_state.input[1] == '\n')
                m_state.input++;

            m_state.addNewline(m_state.input + 1);
            break;
        }

        m_state.input++;
    }
    const char * start = m_state.input;

    // Find end of action (skip trailing spaces, tabulations and new line)
    const char * end = m_state.input;
    while((nBraces > 0) && (m_state.input[0] != '\0'))
    {
        // Increase braces count
        if(m_state.input[0] == '{')
        {
            nBraces++;
            end = m_state.input;
        }
        // Decrease braces count
        else if(m_state.input[0] == '}')
        {
            nBraces--;
            if(nBraces > 0)
                end = m_state.input;
        }
        // Skip unix-style new line
        else if(m_state.input[0] == '\n')
        {
            if(m_state.input[1] == '\r')
                m_state.input++;

            m_state.addNewline(m_state.input + 1);
        }
        // Skip windows-style new line
        else if(m_state.input[0] == '\r')
        {
            if(m_state.input[1] == '\n')
                m_state.input++;

            m_state.addNewline(m_state.input + 1);
        }
        // Skip spaces and tabulations
        else if((m_state.input[0] != ' ') && (m_state.input[0] != '\t'))
            end = m_state.input;

        m_state.input++;
    }

    if(nBraces == 0)
    {
        m_state.input--;
        ruleAction.assign(start, end - start + 1);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
const LexerState & LexerBNF::getState(void)
{
    return m_state;
}

const LexerState & LexerBNF::getLastState(void)
{
    return m_lastState;
}

Token LexerBNF::newToken(const TokenType type)
{
    return { type, m_lastState.input, m_state.input };
}
