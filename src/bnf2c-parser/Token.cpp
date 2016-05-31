////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "Token.h"

#include <cstdlib>
#include <cstring>
#include <iomanip>

#define PARAM_NAME_PREFIX_SIZE 6
#define TYPE_NAME_PREFIX_SIZE  11

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator<<(std::ostream & os, const TokenType tokenType)
{
    switch(tokenType)
    {
        case TokenType::INTERMEDIATE : os << "intermediate";    break;
        case TokenType::TERMINAL     : os << "terminal";        break;
        case TokenType::COMMENT      : os << "comment";         break;
        case TokenType::PARAM_NAME   : os << "parameter name";  break;
        case TokenType::PARAM_VALUE  : os << "parameter value"; break;

        case TokenType::NEW_LINE     : os << "new line";        break;
        case TokenType::AFFECTATION  : os << "affectation";     break;
        case TokenType::OR           : os << "alternative";     break;
        case TokenType::BRACE_OPEN   : os << "open brace";      break;
        case TokenType::BRACE_CLOSE  : os << "close brace";     break;
        case TokenType::EQUAL        : os << "equal";           break;

        case TokenType::END_OF_INPUT : os << "end of input";    break;
        case TokenType::ERROR        : os << "error";           break;
        default                      : os << "Unknown token";   break;
    }

    return os;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator<<(std::ostream & os, const Token & token)
{
    os << std::left << std::setw(17) << token.type;
    os << " ["    << std::right << std::setw(2) << token.valueSize() << "]";
    os << " at L" << std::right << std::setw(2) << token.line << ":C" << std::right << std::setw(2) << token.column;

    switch(token.type)
    {
        case TokenType::INTERMEDIATE : os << " \"" << token.valueToIntermediate()   << "\""; break;
        case TokenType::TERMINAL     : os << " \"" << token.valueToTerminal()       << "\""; break;
        case TokenType::COMMENT      : os << " \"" << token.valueToComment()        << "\""; break;
        case TokenType::PARAM_NAME   : os << " \"" << token.valueToParameterName()  << "\""; break;
        case TokenType::PARAM_VALUE  : os << " \"" << token.valueToParameterValue() << "\""; break;
        default : break;
    }

    return os;
}

////////////////////////////////////////////////////////////////////////////////
long Token::valueSize(void) const
{
    return (end - value);
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::valueToVerbatim(void) const
{
    if((value != nullptr) && (end != nullptr))
        return std::string(value, end - value);
    else
        return std::string("value error");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::valueToComment(void) const
{
    if((value != nullptr) && (end != nullptr))
        return std::string(value, end - value);
    else
        return std::string("value error");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::valueToIntermediate(void) const
{
    if((value != nullptr) && (end != nullptr))
        return std::string(value + 1, end - value - 2);
    else
        return std::string("value error");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::valueToTerminal(void) const
{
    if((value != nullptr) && (end != nullptr))
        return std::string(value, end - value);
    else
        return std::string("value error");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::valueToParameterName(void) const
{
    if((value != nullptr) && (end != nullptr))
        return std::string(value + PARAM_NAME_PREFIX_SIZE, end - (value + PARAM_NAME_PREFIX_SIZE));
    else
        return std::string("value error");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::valueToParameterValue(void) const
{
    if((value != nullptr) && (end != nullptr))
    {
        std::string paramValue(value + 1, end - value - 2);

        std::size_t pos = 0;
        while((pos = paramValue.find("\\\"", pos)) != std::string::npos)
            paramValue.replace(pos, 2, "\"");

        return paramValue;
    }
    else
        return std::string("value error");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::valueToTypeName(void) const
{
    if((value != nullptr) && (end != nullptr))
        return std::string(value + TYPE_NAME_PREFIX_SIZE, end - (value + TYPE_NAME_PREFIX_SIZE) - 1);
    else
        return std::string("value error");
}
