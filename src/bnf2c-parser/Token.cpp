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
    os << std::left << std::setw(17) << token.getType();
    os << " ["    << std::right << std::setw(2) << token.getLength() << "]";

    switch(token.getType())
    {
        case TokenType::INTERMEDIATE : os << " \"" << token.toIntermediate()   << "\""; break;
        case TokenType::TERMINAL     : os << " \"" << token.toTerminal()       << "\""; break;
        case TokenType::COMMENT      : os << " \"" << token.toComment()        << "\""; break;
        case TokenType::PARAM_NAME   : os << " \"" << token.toParameterName()  << "\""; break;
        case TokenType::PARAM_VALUE  : os << " \"" << token.toParameterValue() << "\""; break;
        default : break;
    }

    return os;
}

////////////////////////////////////////////////////////////////////////////////
Token::Token(void)
: type(TokenType::ERROR), start(nullptr), end(nullptr)
{ }

////////////////////////////////////////////////////////////////////////////////
Token::Token(TokenType type, const char * start, const char * end) throw(std::runtime_error)
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
std::string Token::toVerbatim(void) const throw(std::runtime_error)
{
    if((start != nullptr) && (end != nullptr))
        return std::string(start, end - start);
    else
        throw std::runtime_error("Unable to convert token to verbatim text");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::toComment(void) const throw(std::runtime_error)
{
    if((start != nullptr) && (end != nullptr))
        return std::string(start, end - start);
    else
        throw std::runtime_error("Unable to convert token to comment");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::toIntermediate(void) const throw(std::runtime_error)
{
    if((start != nullptr) && (end != nullptr))
        return std::string(start + 1, end - start - 2);
    else
        throw std::runtime_error("Unable to convert token to intermediate name");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::toTerminal(void) const throw(std::runtime_error)
{
    if((start != nullptr) && (end != nullptr))
        return std::string(start, end - start);
    else
        throw std::runtime_error("Unable to convert token to terminal name");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::toParameterName(void) const throw(std::runtime_error)
{
    if((start != nullptr) && (end != nullptr))
        return std::string(start + PARAM_NAME_PREFIX_SIZE, end - (start + PARAM_NAME_PREFIX_SIZE));
    else
        throw std::runtime_error("Unable to convert token to parameter name");
}

////////////////////////////////////////////////////////////////////////////////
std::string Token::toParameterValue(void) const throw(std::runtime_error)
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
std::string Token::toTypeName(void) const throw(std::runtime_error)
{
    if((start != nullptr) && (end != nullptr))
        return std::string(start + TYPE_NAME_PREFIX_SIZE, end - (start + TYPE_NAME_PREFIX_SIZE) - 1);
    else
        throw std::runtime_error("Unable to convert token to type name");
}
