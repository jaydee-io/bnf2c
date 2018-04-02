////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "Token.h"

#include <cstdlib>
#include <cstring>

#define PARAM_NAME_PREFIX_SIZE 6
#define TYPE_NAME_PREFIX_SIZE  11

////////////////////////////////////////////////////////////////////////////////
Token::Token(void)
: type(TokenType::ERROR), start(nullptr), end(nullptr)
{ }

////////////////////////////////////////////////////////////////////////////////
Token::Token(TokenType type, const char * start, const char * end)
: type(type), start(start), end(end)
{
    if(!start)
        throw std::runtime_error("Wrong start of token");

    if(!end)
        throw std::runtime_error("Wrong end of token");

    if(start > end)
        throw std::runtime_error("Inverted start/end of token");
}

////////////////////////////////////////////////////////////////////////////////
bool Token::operator ==(const Token & token) const
{
    return type == token.type && start == token.start && end == token.end;
}

////////////////////////////////////////////////////////////////////////////////
bool Token::operator !=(const Token & token) const
{
    return !(*this == token);
}

////////////////////////////////////////////////////////////////////////////////
TokenType Token::getType(void) const
{
    return type;
}

////////////////////////////////////////////////////////////////////////////////
unsigned long Token::getLength(void) const
{
    return (end - start);
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::toVerbatim(void) const
{
    if((start != nullptr) && (end != nullptr))
        return std::string(start, end - start);
    else
        throw std::runtime_error("Unable to convert token to verbatim text");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::toComment(void) const
{
    if((start != nullptr) && (end != nullptr))
        return std::string(start, end - start);
    else
        throw std::runtime_error("Unable to convert token to comment");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::toIntermediate(void) const
{
    if((start != nullptr) && (end != nullptr))
        return std::string(start + 1, end - start - 2);
    else
        throw std::runtime_error("Unable to convert token to intermediate name");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::toTerminal(void) const
{
    if((start != nullptr) && (end != nullptr))
        return std::string(start, end - start);
    else
        throw std::runtime_error("Unable to convert token to terminal name");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::toParameterName(void) const
{
    if((start != nullptr) && (end != nullptr))
        return std::string(start + PARAM_NAME_PREFIX_SIZE, end - (start + PARAM_NAME_PREFIX_SIZE));
    else
        throw std::runtime_error("Unable to convert token to parameter name");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::toParameterValue(void) const
{
    if((start != nullptr) && (end != nullptr))
    {
        std::string paramValue(start + 1, end - start - 2);

        std::size_t pos = 0;
        while((pos = paramValue.find("\\\"", pos)) != std::string::npos)
            paramValue.replace(pos, 2, "\"");

        return paramValue;
    }
    else
        throw std::runtime_error("Unable to convert token to parameter value");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::toTypeName(void) const
{
    if((start != nullptr) && (end != nullptr))
        return std::string(start + TYPE_NAME_PREFIX_SIZE, end - (start + TYPE_NAME_PREFIX_SIZE) - 1);
    else
        throw std::runtime_error("Unable to convert token to type name");
}
