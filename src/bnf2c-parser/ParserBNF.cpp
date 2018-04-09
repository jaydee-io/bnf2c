////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "ParserBNF.h"
#include "LexerBNF.h"
#include "core/Grammar.h"
#include "core/Rule.h"
#include "config/ParameterizedString.h"
#include "printer/PrettyPrinters.h"

#include <sstream>

#define ADD_PARSING_ERROR(message)\
    do {\
        std::stringstream ss;\
        ss << message;\
        errors.list.push_back(ParsingError(m_lexer.getLastState(), m_token.getLength(), ss.str()));\
    } while(false)


////////////////////////////////////////////////////////////////////////////////
ParserBNF::ParserBNF(LexerBNF & lexer, Grammar & grammar)
: m_grammar(grammar), m_lexer(lexer)
{
    // Parser options
    m_stringParams["parser:parser-type"]            = &m_options.parserType;

    m_stringParams["parser:state-type"]            = &m_options.stateType;
    m_stringParams["parser:top-state"]             = &m_options.topState;
    m_parameterizedStringParams["parser:pop-state"] = &m_options.popState;
    m_stringParams["parser:error-state"]           = &m_options.errorState;
    m_stringParams["parser:accept-state"]          = &m_options.acceptState;

    m_stringParams["parser:value-type"]            = &m_options.valueType;
    m_parameterizedStringParams["parser:push-value"] = &m_options.pushValue;
    m_parameterizedStringParams["parser:pop-values"] = &m_options.popValues;
    m_parameterizedStringParams["parser:get-value"] = &m_options.getValue;
    m_parameterizedStringParams["parser:value-as-token"] = &m_options.valueAsToken;
    m_parameterizedStringParams["parser:value-as-intermediate"] = &m_options.valueAsIntermediate;
    m_parameterizedStringParams["parser:default-action"] = &m_options.defaultAction;

    // Lexer options
    m_stringParams["lexer:token-type"]          = &m_options.tokenType;
    m_stringParams["lexer:shift-token"]         = &m_options.shiftToken;
    m_stringParams["lexer:token-prefix"]        = &m_options.tokenPrefix;
    m_parameterizedStringParams["lexer:get-type-of-token"] = &m_options.getTypeOfToken;
    m_stringParams["lexer:end-of-input-token"]  = &m_options.endOfInputToken;

    // Generated code options
    m_stringParams["output:intermediate-type"]  = &m_options.intermediateType;
    m_stringParams["output:parse-function"]     = &m_options.parseFunctionName;
    m_stringParams["output:branch-function"]    = &m_options.branchFunctionName;
    m_stringParams["output:throwed-exceptions"] = &m_options.throwedExceptions;

    m_boolParams  ["generator:default-switch"]  = &m_options.defaultSwitchStatement;
    m_boolParams  ["generator:branch-table"]    = &m_options.useTableForBranches;

    // Internal options
    m_stringParams["indent:string"]             = &m_options.indent.string;
    m_uintParams  ["indent:top"]                = &m_options.indent.top;
}

////////////////////////////////////////////////////////////////////////////////
void ParserBNF::parseBnf2cBlock(void)
{
    m_token = m_lexer.nextToken();

    for(;;)
    {
        switch(m_token.getType())
        {
            // New parsing rule
            case TokenType::INTERMEDIATE :
            {
                m_lastIntermediate = m_token.toIntermediate();
                Rule rule(m_lastIntermediate);
                m_grammar.intermediates.insert(m_lastIntermediate);

                m_token = m_lexer.nextToken();
                if(m_token.getType() != TokenType::AFFECTATION)
                    ADD_PARSING_ERROR("Expected " << TokenType::AFFECTATION << " '::=' but got a " << m_token.getType());

                parseRule(rule);

                m_grammar.addRule(rule);
                continue;
            }

            // New alternative to the current parsing rule
            case TokenType::OR :
            {
                Rule rule(m_lastIntermediate);
                m_grammar.intermediates.insert(m_lastIntermediate);

                parseRule(rule);

                m_grammar.addRule(rule);
                continue;
            }

            // Intermediate type
            case TokenType::TYPE_NAME :
            {
                parseIntermediatesTypes();
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
                ADD_PARSING_ERROR("Unexpected " << m_token.getType());
                break;

            // Lexer error
            case TokenType::ERROR :
                ADD_PARSING_ERROR("Unknown '" << m_token.toVerbatim() << "'");
                break;

            // End of input
            case TokenType::END_OF_INPUT :
                return;
        }

        m_token = m_lexer.nextToken();
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParserBNF::parseRule(Rule & rule)
{
    bool endOfRule = false;

    for(;;)
    {
        m_token = m_lexer.nextToken();

        switch(m_token.getType())
        {
            case TokenType::INTERMEDIATE :
                if(endOfRule)
                    return;
                rule.addSymbol(m_grammar.addIntermediate(m_token.toIntermediate()));
                break;

            case TokenType::TERMINAL :
                if(endOfRule)
                    return;
                rule.addSymbol(m_grammar.addTerminal(m_token.toTerminal()));
                break;

            // Rule action
            case TokenType::BRACE_OPEN :
            {
                m_lexer.readRuleAction(rule.action);

                m_token = m_lexer.nextToken();
                if(m_token.getType() != TokenType::BRACE_CLOSE)
                    ADD_PARSING_ERROR("Expected " << TokenType::BRACE_CLOSE << " '}' but got " << m_token.getType());

                endOfRule = true;

                break;
            }

            case TokenType::NEW_LINE :
                endOfRule = true;
                break;

            case TokenType::AFFECTATION :
                ADD_PARSING_ERROR("Expected terminal or intermediate name but got " << m_token.getType());
                break;

            case TokenType::ERROR :
                ADD_PARSING_ERROR("Unknown '" << m_token.toVerbatim() << "'");
                break;

            default :
                return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParserBNF::parseParameter(void)
{
    std::string paramName = m_token.toParameterName();
    auto paramNameToken = m_token;
    auto paramNameLocation = m_lexer.getLastState();

    // Equal token
    m_token = m_lexer.nextToken();
    while(m_token.getType() == TokenType::NEW_LINE)
        m_token = m_lexer.nextToken();
    if(m_token.getType() != TokenType::EQUAL)
        ADD_PARSING_ERROR("Expected " << TokenType::EQUAL << " '=' but got " << m_token.getType());

    // Parameter value
    m_token = m_lexer.nextToken();
    while(m_token.getType() == TokenType::NEW_LINE)
        m_token = m_lexer.nextToken();
    if(m_token.getType() != TokenType::PARAM_VALUE)
        ADD_PARSING_ERROR("Expected " << TokenType::PARAM_VALUE << " but got " << m_token.getType());

    std::stringstream ss(m_token.toParameterValue());

    // Lookup in parameterized string map
    ParameterizedStringParamMap::iterator itParameterizedString = m_parameterizedStringParams.find(paramName);
    if(itParameterizedString != m_parameterizedStringParams.end())
    {
        (*itParameterizedString->second) = ss.str();

        // Check "popState" special parameter
        if(m_options.popState.toString().find(Vars::NB_STATES) == std::string::npos)
            ADD_PARSING_ERROR("Parameter \"" << paramName << "\" must contains the keyword " << Vars::NB_STATES << " to be replaced by the number of states to be poped");

        return;
    }

    // Lookup in string map
    StringParamMap::iterator itString = m_stringParams.find(paramName);
    if(itString != m_stringParams.end())
    {
        (*itString->second) = ss.str();

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
            ADD_PARSING_ERROR("Parameter \"" << paramName << "\" conversion failure, value must be boolean");

        return;
    }

    // Lookup in unsigned int map
    UintParamMap::iterator itUint = m_uintParams.find(paramName);
    if(itUint != m_uintParams.end())
    {
        ss >> (*itUint->second);

        if(ss.fail())
            ADD_PARSING_ERROR("Parameter \"" << paramName << "\" conversion failure, value must be numerical");

        return;
    }

    errors.list.push_back(ParsingError(paramNameLocation, paramNameToken.getLength(), "Unknown parameter \"" + paramName + "\""));
}

////////////////////////////////////////////////////////////////////////////////
void ParserBNF::parseIntermediatesTypes(void)
{
    std::string type = m_token.toTypeName();

    for(;;)
    {
        m_token = m_lexer.nextToken();

        switch(m_token.getType())
        {
            // Intermediate names
            case TokenType::TERMINAL :
                m_grammar.intermediateTypes[m_token.toTerminal()] = type;
                break;

            default :
                return;
        }
    }
}
