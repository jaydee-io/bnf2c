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
#ifndef __TOKEN_H__
#define __TOKEN_H__
#include <string>
#include <ostream>

enum class TokenType
{
    INTERMEDIATE,
    TERMINAL,
    COMMENT,
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
    friend std::ostream & operator<<(std::ostream & os, const Token & token);


    TokenType    type;
    const char * value;
    const char * end;
    int          line;
    int          column;


    long        valueSize(void) const;

    std::string valueToComment(void) const;
    std::string valueToIntermediate(void) const;
    std::string valueToTerminal(void) const;
    std::string valueToParameterName(void) const;
    std::string valueToParameterValue(void) const;
};

std::ostream & operator<<(std::ostream & os, const TokenType tokenType);
std::ostream & operator<<(std::ostream & os, const Token & token);

#endif /* __TOKEN_H__ */
