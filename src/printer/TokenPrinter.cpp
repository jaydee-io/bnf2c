////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "PrettyPrinters.h"
#include "bnf2c-parser/Token.h"

#include <iomanip>

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
