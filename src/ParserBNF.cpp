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
#include "ParserBNF.h"

#include <iomanip>
#include <sstream>

#define COLOR_RED   "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RESET "\033[0m"

#define THROW_PARSING_ERROR(message)\
    do {\
        std::stringstream ss;\
        ss << message;\
        throw ParsingError({m_lexer, m_token, ss.str()});\
    } while(false)

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const ParsingError & error)
{
    std::string currentLine = error.lexer.getCurrentLine();

    os << COLOR_RED "Parsing error" COLOR_RESET " at L" << error.token.line << ":C" << error.token.column << " : " << error.message << std::endl;
    os << currentLine << std::endl;
    if(error.token.valueSize() > 1)
    {
        for(int i=0; i<error.lexer.getTabulations(); i++)
            os << '\t';
        os << COLOR_GREEN << std::setw(error.token.column - error.lexer.getTabulations()) << std::setfill(' ') << std::right << '^';
        os << std::setw(error.token.valueSize() - 1) << std::setfill('-') << std::right << '^' << COLOR_RESET << std::endl;
    }
    else
    {
        for(int i=0; i<error.lexer.getTabulations(); i++)
            os << '\t';
        os << COLOR_GREEN << std::setw(error.token.column - error.lexer.getTabulations()) << std::right << '^' << COLOR_RESET << std::endl;
    }

    return os;
}

////////////////////////////////////////////////////////////////////////////////
ParserBNF::ParserBNF(LexerBNF & lexer, Options & options, Grammar & grammar)
: m_grammar(grammar), m_options(options), m_lexer(lexer)
{
    m_stringParams["bnf2c:state:top"]               = &m_options.topState;
    m_stringParams["bnf2c:state:pop"]               = &m_options.popState;
    m_stringParams["bnf2c:state:error"]             = &m_options.errorState;
    m_stringParams["bnf2c:state:accept"]            = &m_options.acceptState;

    m_stringParams["bnf2c:type:state"]              = &m_options.stateType;
    m_stringParams["bnf2c:type:token"]              = &m_options.tokenType;
    m_stringParams["bnf2c:type:intermediate"]       = &m_options.intermediateType;

    m_stringParams["bnf2c:token:shift"]             = &m_options.shiftToken;
    m_stringParams["bnf2c:token:prefix"]            = &m_options.tokenPrefix;
    m_stringParams["bnf2c:token:endOfInput"]        = &m_options.endOfInputToken;

    m_boolParams  ["bnf2c:generator:defaultSwitch"] = &m_options.defaultSwitchStatement;
    m_boolParams  ["bnf2c:generator:branchTable"]   = &m_options.useTableForBranches;

    m_stringParams["bnf2c:output:parseFunction"]    = &m_options.parseFunctionName;
    m_stringParams["bnf2c:output:branchFunction"]   = &m_options.branchFunctionName;
    m_stringParams["bnf2c:output:exceptions"]       = &m_options.throwedExceptions;

    m_stringParams["bnf2c:indent:string"]           = &m_options.indent.string;
    m_uintParams  ["bnf2c:indent:top"]              = &m_options.indent.top;
}

////////////////////////////////////////////////////////////////////////////////
void ParserBNF::parseBnf2cBlock(void) throw(ParsingError)
{
    m_lexer.nextToken(m_token);

    for(;;)
    {
        switch(m_token.type)
        {
            // New parsing rule
            case TokenType::INTERMEDIATE :
            {
                m_lastIntermediate = m_token.valueToIntermediate();
                Rule rule(m_grammar.intermediates.add(m_lastIntermediate));

                m_lexer.nextToken(m_token);
                if(m_token.type != TokenType::AFFECTATION)
                    THROW_PARSING_ERROR("Expected token " << TokenType::AFFECTATION << " but got " << m_token.type);

                parseRule(rule);

                m_grammar.addRule(rule);
                continue;
                break;
            }

            // New alternative to the current parsing rule
            case TokenType::OR :
            {
                Rule rule(m_grammar.intermediates.add(m_lastIntermediate));

                parseRule(rule);

                m_grammar.addRule(rule);
                continue;
                break;
            }

            // Parameter
            case TokenType::PARAM_NAME :
            {
                parseParameter();
                break;
            }

            // Skip comment and new lines
            case TokenType::COMMENT  :
            case TokenType::NEW_LINE :
                break;

            // Unexpected tokens
            case TokenType::AFFECTATION :
            case TokenType::TERMINAL :
            case TokenType::BRACE_OPEN :
            case TokenType::BRACE_CLOSE :
            case TokenType::PARAM_VALUE :
            case TokenType::EQUAL :
                THROW_PARSING_ERROR("Unexpected token " << m_token.type);
                break;

            // Lexer error
            case TokenType::ERROR :
                THROW_PARSING_ERROR("Lexer error");
                break;

            // End of input
            case TokenType::END_OF_INPUT :
                return;
                break;
        }

        m_lexer.nextToken(m_token);
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParserBNF::parseRule(Rule & rule) throw(ParsingError)
{
    bool endOfRule = false;

    for(;;)
    {
        m_lexer.nextToken(m_token);

        switch(m_token.type)
        {
            case TokenType::INTERMEDIATE :
                if(endOfRule)
                    return;
                rule.addSymbol(m_grammar.addIntermediate(m_token.valueToIntermediate()));
                break;

            case TokenType::TERMINAL :
                if(endOfRule)
                    return;
                rule.addSymbol(m_grammar.addTerminal(m_token.valueToTerminal()));
                break;

            // Rule action
            case TokenType::BRACE_OPEN :
            {
                m_lexer.readRuleAction(rule.action);

                m_lexer.nextToken(m_token);
                if(m_token.type != TokenType::BRACE_CLOSE)
                    THROW_PARSING_ERROR("Expected token " << TokenType::BRACE_CLOSE << " but got " << m_token.type);

                endOfRule = true;

                break;
            }

            case TokenType::NEW_LINE :
                endOfRule = true;
                break;

            case TokenType::AFFECTATION :
                THROW_PARSING_ERROR("Unexpected token " << m_token.type);
                break;

            case TokenType::ERROR :
                THROW_PARSING_ERROR("Lexer error");
                break;

            default :
                return;
                break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParserBNF::parseParameter(void) throw(ParsingError)
{
    Token           backupToken = m_token;
    LexerBNF::State backupState = m_lexer.saveState();
    std::string paramName = m_token.valueToParameterName();

    m_lexer.nextToken(m_token);
    while(m_token.type == TokenType::NEW_LINE)
        m_lexer.nextToken(m_token);
    if(m_token.type != TokenType::EQUAL)
        THROW_PARSING_ERROR("Expected token " << TokenType::EQUAL << " but got " << m_token.type);

    m_lexer.nextToken(m_token);
    while(m_token.type == TokenType::NEW_LINE)
        m_lexer.nextToken(m_token);
    if(m_token.type != TokenType::PARAM_VALUE)
        THROW_PARSING_ERROR("Expected token " << TokenType::PARAM_VALUE << " but got " << m_token.type);

    std::stringstream ss(m_token.valueToParameterValue());

    // Lookup in string map
    StringParamMap::iterator itString = m_stringParams.find(paramName);
    if(itString != m_stringParams.end())
    {
        (*itString->second) = ss.str();

        // Check "popState" special parameter
        if(m_options.popState.find(Options::VAR_NB_STATES) == std::string::npos)
            THROW_PARSING_ERROR("Parameter \"" << paramName << "\" must contains the keyword " << Options::VAR_NB_STATES << " to be replaced by the number of states to be poped");

        return;
    }

    // Lookup in bool map
    BoolParamMap::iterator itBool = m_boolParams.find(paramName);
    if(itBool != m_boolParams.end())
    {
        if(std::isdigit(ss.str()[0]))
            ss >> (*itBool->second);
        else
            ss >> std::boolalpha >> (*itBool->second);

        if(ss.fail())
            THROW_PARSING_ERROR("Parameter \"" << paramName << "\" conversion failure, value must be boolean");

        return;
    }

    // Lookup in unsigned int map
    UintParamMap::iterator itUint = m_uintParams.find(paramName);
    if(itUint != m_uintParams.end())
    {
        ss >> (*itUint->second);

        if(ss.fail())
            THROW_PARSING_ERROR("Parameter \"" << paramName << "\" conversion failure, value must be numerical");

        return;
    }

    m_token = backupToken;
    m_lexer.restoreState(backupState);
    THROW_PARSING_ERROR("Unknown parameter \"" << paramName << "\"");
}
