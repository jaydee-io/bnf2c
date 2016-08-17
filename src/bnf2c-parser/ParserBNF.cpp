////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "ParserBNF.h"

#include <sstream>

#define ADD_PARSING_ERROR(message)\
    do {\
        std::stringstream ss;\
        ss << message;\
        errors.list.push_back(ParsingError({m_token.line, m_token.column, m_lexer.getTabulations(), m_lexer.getCurrentLine(), m_token.valueToVerbatim(), ss.str()}));\
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
    m_lexer.nextToken(m_token);

    for(;;)
    {
        switch(m_token.type)
        {
            // New parsing rule
            case TokenType::INTERMEDIATE :
            {
                m_lastIntermediate = m_token.valueToIntermediate();
                Rule rule(m_lastIntermediate);
                m_grammar.intermediates.insert(m_lastIntermediate);

                m_lexer.nextToken(m_token);
                if(m_token.type != TokenType::AFFECTATION)
                    ADD_PARSING_ERROR("Expected " << TokenType::AFFECTATION << " '::=' but got a " << m_token.type);

                parseRule(rule);

                m_grammar.addRule(rule);
                continue;
                break;
            }

            // New alternative to the current parsing rule
            case TokenType::OR :
            {
                Rule rule(m_lastIntermediate);
                m_grammar.intermediates.insert(m_lastIntermediate);

                parseRule(rule);

                m_grammar.addRule(rule);
                continue;
                break;
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
                ADD_PARSING_ERROR("Unexpected " << m_token.type);
                break;

            // Lexer error
            case TokenType::ERROR :
                ADD_PARSING_ERROR("Unknown '" << m_token.valueToVerbatim() << "'");
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
void ParserBNF::parseRule(Rule & rule)
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
                    ADD_PARSING_ERROR("Expected " << TokenType::BRACE_CLOSE << " '}' but got " << m_token.type);

                endOfRule = true;

                break;
            }

            case TokenType::NEW_LINE :
                endOfRule = true;
                break;

            case TokenType::AFFECTATION :
                ADD_PARSING_ERROR("Expected terminal or intermediate name but got " << m_token.type);
                break;

            case TokenType::ERROR :
                ADD_PARSING_ERROR("Unknown '" << m_token.valueToVerbatim() << "'");
                break;

            default :
                return;
                break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParserBNF::parseParameter(void)
{
    std::string paramName = m_token.valueToParameterName();

    // Equal token
    m_lexer.nextToken(m_token);
    while(m_token.type == TokenType::NEW_LINE)
        m_lexer.nextToken(m_token);
    if(m_token.type != TokenType::EQUAL)
        ADD_PARSING_ERROR("Expected " << TokenType::EQUAL << " '=' but got " << m_token.type);

    // Parameter value
    m_lexer.nextToken(m_token);
    while(m_token.type == TokenType::NEW_LINE)
        m_lexer.nextToken(m_token);
    if(m_token.type != TokenType::PARAM_VALUE)
        ADD_PARSING_ERROR("Expected " << TokenType::PARAM_VALUE << " but got " << m_token.type);

    std::stringstream ss(m_token.valueToParameterValue());

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

    ADD_PARSING_ERROR("Unknown parameter \"" << paramName << "\"");
}

////////////////////////////////////////////////////////////////////////////////
void ParserBNF::parseIntermediatesTypes(void)
{
    std::string type = m_token.valueToTypeName();

    for(;;)
    {
        m_lexer.nextToken(m_token);

        switch(m_token.type)
        {
            // Intermediate names
            case TokenType::TERMINAL :
                m_grammar.intermediateTypes[m_token.valueToTerminal()] = type;
                break;

            default :
                return;
                break;
        }
    }
}
