////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _TOKEN_H_
#define _TOKEN_H_
#include <string>
#include <ostream>

enum class TokenType
{
    INTERMEDIATE,
    TERMINAL,
    COMMENT,
    TYPE_NAME,
    PARAM_NAME,
    PARAM_VALUE,

    NEW_LINE,
    AFFECTATION,
    OR,
    BRACE_OPEN,
    BRACE_CLOSE,
    EQUAL,

    END_OF_INPUT,
    ERROR
};

struct Token
{
    TokenType    type;
    const char * value;
    const char * end;
    int          line;
    int          column;


    long        valueSize(void) const;

    std::string valueToVerbatim(void) const;
    std::string valueToComment(void) const;
    std::string valueToIntermediate(void) const;
    std::string valueToTerminal(void) const;
    std::string valueToParameterName(void) const;
    std::string valueToParameterValue(void) const;
    std::string valueToTypeName(void) const;
};

std::ostream & operator<<(std::ostream & os, const TokenType tokenType);
std::ostream & operator<<(std::ostream & os, const Token & token);

#endif /* _TOKEN_H_ */
