////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _TOKEN_H_
#define _TOKEN_H_
#include <string>
#include <stdexcept>

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

class Token
{
    public :
        Token(void);
        Token(TokenType type, const char * start, const char * end);

        bool operator ==(const Token & token) const;
        bool operator !=(const Token & token) const;

        TokenType     getType(void) const;
        unsigned long getLength(void) const;

        std::string toVerbatim(void) const;
        std::string toComment(void) const;
        std::string toIntermediate(void) const;
        std::string toTerminal(void) const;
        std::string toParameterName(void) const;
        std::string toParameterValue(void) const;
        std::string toTypeName(void) const;

    private :
        TokenType    type;
        const char * start;
        const char * end;
};

#endif /* _TOKEN_H_ */
