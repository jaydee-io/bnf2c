////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "gtest/gtest.h"
#include <iostream>
#include <stack>
#include <deque>

namespace calc {
/*!bnf2c
   bnf2c:parser:top-state             = "calc::stateStack.top()"
   bnf2c:parser:pop-state             = "for(int i=0; i<<NB_STATES>; i++) calc::stateStack.pop();"
   bnf2c:parser:error-state           = "STATE_ERROR"
   bnf2c:parser:accept-state          = "STATE_ACCEPT"

   bnf2c:parser:value-type            = "calc::Value"
   bnf2c:parser:push-value            = "calc::push_value(calc::Value(<VALUE>));"
   bnf2c:parser:pop-values            = "calc::pop_values(<NB_VALUES>);"
   bnf2c:parser:get-value             = "calc::get_value(<VALUE_IDX>)"
   bnf2c:parser:value-as-token        = "<VALUE>.token"
   bnf2c:parser:value-as-intermediate = "<VALUE>.<TYPE>"

   bnf2c:lexer:token-type             = "calc::Token"
   bnf2c:lexer:shift-token            = "nextToken();"
   bnf2c:lexer:token-prefix           = "calc::"
   bnf2c:lexer:get-type-of-token      = "<TOKEN>.type"
   bnf2c:lexer:end-of-input-token     = "EOI"

   bnf2c:output:intermediate-type     = "long long"
   bnf2c:output:parse-function        = "calc::parseFunction"
   bnf2c:output:branch-function       = "branchFunction"

   bnf2c:generator:default-switch     = "true"
   bnf2c:generator:branch-table       = "false"

   bnf2c:indent:string                = "    "
   bnf2c:indent:top                   = "0"

   bnf2c:type<value> E START
*/

typedef enum {
    MULT,
    DIV,
    ADD,
    SUB,
    NUMBER,
    EOI,
    ERROR
} T_TOKEN;

struct Token
{
    T_TOKEN type;
    const char *  start;

    long long number(void) const { return ::atoll(start); }
};

union Value
{
    long long   value;
    Token       token;

    Value(void) { }
    Value(const Token & token) : token(token) { }
};

int parseFunction(Token);

#define STATE_ERROR  -5
#define STATE_ACCEPT -6

std::stack<int>     stateStack;
std::deque<Value>   valueStack;

const char * input;

Token token;


const char * tokenName [] = {
    "MULT",
    "DIV",
    "ADD",
    "SUB",
    "NUMBER",
    "EOI",
    "ERROR"
};

void push_value(const Value & value)
{
    valueStack.push_back(value);
}

void pop_values(int nbValues)
{
    valueStack.resize(valueStack.size() - nbValues);
}

Value & get_value(int idx)
{
    return valueStack[valueStack.size() - idx - 1];
}

void printStatesStack(void)
{
    std::stack<int> localStack(stateStack);

    std::cout << "States = ";
    while(!localStack.empty())
    {
        std::cout << ' ' << localStack.top();
        localStack.pop();
    }
    std::cout << std::endl;
}


void nextToken(void)
{
    for(;;)
    {
        token.start = input;

        /*!re2c
        re2c:define:YYCTYPE = "char";
        re2c:define:YYCURSOR = input;
        re2c:define:YYMARKER = marker;
        re2c:indent:top = 1;
        re2c:indent:string = "    ";
        re2c:yyfill:enable = 0;

        [ \t]+ { continue; }

        "*"    { token.type = MULT; break; }
        "/"    { token.type = DIV;  break; }
        "+"    { token.type = ADD;  break; }
        "-"    { token.type = SUB;  break; }

        [0-9]+ { token.type = NUMBER; break; }

        "\000" { token.type = EOI;   break; }
        [^]    { token.type = ERROR; break; }
        */
    }
}

/*!bnf2c
<START> ::= <E>

<E> ::= NUMBER       { $$ = $1.number(); }
      | <E> MULT <E> { $$ = $1 * $3;     }
      | <E> DIV  <E> { $$ = $1 / $3;     }
      | <E> ADD  <E> { $$ = $1 + $3;     }
      | <E> SUB  <E> { $$ = $1 - $3;     }
*/

TEST(Calc, AddSubMultiplyDivide)
{
    calc::input = "1+0+1*3+50/2+9+1+1-10";

    calc::nextToken();
    calc::stateStack.push(0);
    while((calc::stateStack.top() != STATE_ERROR) && (calc::stateStack.top() != STATE_ACCEPT))
        calc::stateStack.push(calc::parseFunction(calc::token));

    EXPECT_EQ(STATE_ACCEPT, calc::stateStack.top()) << "An error has occured while parsing expression";
    EXPECT_EQ(20, calc::valueStack.back().value);
}

} /* Namespace calc */
