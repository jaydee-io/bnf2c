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
   re2c:define:YYCURSOR = m_input;
   re2c:define:YYMARKER = marker;
   re2c:indent:top = 1;
   re2c:indent:string = "    ";
   re2c:yyfill:enable = 0;
*/

const std::string LexerBNF::BNF2C_TOKEN("/*!bnf2c");

////////////////////////////////////////////////////////////////////////////////
LexerBNF::LexerBNF(const std::string & input, std::ostream & output)
: m_input(input.c_str()), m_lastNewLine(input.c_str()), m_line(1), m_tabs(0), m_output(output)
{
}

////////////////////////////////////////////////////////////////////////////////
bool LexerBNF::moveToNextBnf2cBlock(void)
{
    const char * start = m_input;

    while((m_input[0] != '\0') && (LexerBNF::BNF2C_TOKEN.compare(0, std::string::npos, m_input, LexerBNF::BNF2C_TOKEN.size()) != 0))
    {
        if(m_input[0] == '\n')
        {
            if(m_input[1] == '\r')
                m_input++;

            m_lastNewLine = m_input + 1;
            m_line++;
        }
        else if(m_input[0] == '\r')
        {
            if(m_input[1] == '\n')
                m_input++;

            m_lastNewLine = m_input + 1;
            m_line++;
        }

        m_input++;
    }

    m_output.write(start, m_input - start);
    if(m_input[0] != '\0')
        m_input += LexerBNF::BNF2C_TOKEN.size();

    return (m_input[0] != '\0');
}

////////////////////////////////////////////////////////////////////////////////
void LexerBNF::nextToken(Token & token)
{
    const char * marker;

    for (;;)
    {
        token.value  = m_input;
        token.line   = m_line;
        token.column = getColumn();

        /*!re2c
        identifier = [a-zA-Z0-9_\-]+;
        paramName  = [a-zA-Z0-9_\-:]+;
        typeName   = [a-zA-Z0-9_\-\.:<>]+;

        [ ]+ { continue; }
        [\t] { m_tabs++; continue; }

        "\n\r"|"\r\n"|"\n"|"\r"  { token.type = TokenType::NEW_LINE; m_line++; m_lastNewLine = m_input; m_tabs = 0; break; }
        "::="                    { token.type = TokenType::AFFECTATION; break; }
        "|"                      { token.type = TokenType::OR;          break; }
        "{"                      { token.type = TokenType::BRACE_OPEN;  break; }
        "}"                      { token.type = TokenType::BRACE_CLOSE; break; }
        "="                      { token.type = TokenType::EQUAL;       break; }

        "<"identifier">"         { token.type = TokenType::INTERMEDIATE; break; }
        identifier               { token.type = TokenType::TERMINAL  ;   break; }
        "#"[^\n]*                { token.type = TokenType::COMMENT;      break; }
        "bnf2c:type<"typeName">" { token.type = TokenType::TYPE_NAME;    break; }
        "bnf2c:"paramName        { token.type = TokenType::PARAM_NAME;   break; }
        '"'("\\\""|[^"])*'"'     { token.type = TokenType::PARAM_VALUE;  break; }

        "*\/"  {                  token.type = TokenType::END_OF_INPUT; break; }
        "\000" { m_input--; token.type = TokenType::END_OF_INPUT; break; }
        [^]    {                  token.type = TokenType::ERROR;        break; }
        */
    }

    token.end = m_input;
}

////////////////////////////////////////////////////////////////////////////////
bool LexerBNF::readRuleAction(std::string & ruleAction)
{
    int nBraces = 1;

    // Find start of action
    while((nBraces > 0) && (m_input[0] != '\0'))
    {
        // Skip unix-style new line
        if(m_input[0] == '\n')
        {
            if(m_input[1] == '\r')
                m_input++;

            m_lastNewLine = m_input + 1;
            m_line++;
            m_input++;
            break;
        }
        // Skip windows-style new line
        else if(m_input[0] == '\r')
        {
            if(m_input[1] == '\n')
                m_input++;

            m_lastNewLine = m_input + 1;
            m_line++;
            m_input++;
            break;
        }
        // Skip spaces and tabulations
        if((m_input[0] != ' ') && (m_input[0] != '\t'))
            break;

        m_input++;
    }
    const char * start = m_input;

    // Find end of action (skip trailing spaces, tabulations and new line)
    const char * end = m_input;
    while((nBraces > 0) && (m_input[0] != '\0'))
    {
        // Increase braces count
        if(m_input[0] == '{')
        {
            nBraces++;
            end = m_input;
        }
        // Decrease braces count
        else if(m_input[0] == '}')
        {
            nBraces--;
            if(nBraces > 0)
                end = m_input;
        }
        // Skip unix-style new line
        else if(m_input[0] == '\n')
        {
            if(m_input[1] == '\r')
                m_input++;

            m_lastNewLine = m_input + 1;
            m_line++;
        }
        // Skip windows-style new line
        else if(m_input[0] == '\r')
        {
            if(m_input[1] == '\n')
                m_input++;

            m_lastNewLine = m_input + 1;
            m_line++;
        }
        // Skip spaces and tabulations
        else if((m_input[0] != ' ') && (m_input[0] != '\t'))
            end = m_input;

        m_input++;
    }

    if(nBraces == 0)
    {
        m_input--;
        ruleAction.assign(start, end - start + 1);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
int LexerBNF::getLine(void) const
{
    return m_line;
}

////////////////////////////////////////////////////////////////////////////////
int LexerBNF::getColumn(void) const
{
    return m_input - m_lastNewLine + 1;
}

////////////////////////////////////////////////////////////////////////////////
int LexerBNF::getTabulations(void) const
{
    return m_tabs;
}

////////////////////////////////////////////////////////////////////////////////
std::string LexerBNF::getCurrentLine(void) const
{
    std::string currentLine;
    const char * endLine = m_lastNewLine;

    while((endLine[0] != '\0') && (endLine[0] != '\n') && (endLine[0] != '\r'))
        endLine++;

    currentLine.assign(m_lastNewLine, endLine - m_lastNewLine);

    return currentLine;
}
