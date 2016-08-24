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
        Token(TokenType type, const char * start, const char * end) throw(std::runtime_error);

        bool operator ==(const Token & token) const;
        bool operator !=(const Token & token) const;

        TokenType     getType(void) const;
        unsigned long getLength(void) const;

        std::string toVerbatim(void) const throw(std::runtime_error);
        std::string toComment(void) const throw(std::runtime_error);
        std::string toIntermediate(void) const throw(std::runtime_error);
        std::string toTerminal(void) const throw(std::runtime_error);
        std::string toParameterName(void) const throw(std::runtime_error);
        std::string toParameterValue(void) const throw(std::runtime_error);
        std::string toTypeName(void) const throw(std::runtime_error);

    private :
        TokenType    type;
        const char * start;
        const char * end;
};

std::ostream & operator<<(std::ostream & os, const TokenType tokenType);
std::ostream & operator<<(std::ostream & os, const Token & token);

#endif /* _TOKEN_H_ */
