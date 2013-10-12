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
        case TokenType::INTERMEDIATE : os << "INTERMEDIATE";  break;
        case TokenType::TERMINAL     : os << "TERMINAL";      break;
        case TokenType::COMMENT      : os << "COMMENT";       break;
        case TokenType::PARAM_NAME   : os << "PARAM_NAME";    break;
        case TokenType::PARAM_VALUE  : os << "PARAM_VALUE";   break;

        case TokenType::NEW_LINE     : os << "NEW_LINE";      break;
        case TokenType::AFFECTATION  : os << "AFFECTATION";   break;
        case TokenType::OR           : os << "OR";            break;
        case TokenType::BRACE_OPEN   : os << "BRACE_OPEN";    break;
        case TokenType::BRACE_CLOSE  : os << "BRACE_CLOSE";   break;
        case TokenType::EQUAL        : os << "EQUAL";         break;

        case TokenType::END_OF_INPUT : os << "END_OF_INPUT";  break;
        case TokenType::ERROR        : os << "ERROR";         break;
        default                      : os << "Unknown token"; break;
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
