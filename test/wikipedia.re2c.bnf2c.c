/*******************************************************************************
*                                     BNF2C
* 
*  This file is distributed under the 4-clause Berkeley Software Distribution
*  License. See LICENSE for details.
*******************************************************************************/
#include "wikipedia.h"
#include <stdio.h>

/*!bnf2c
   bnf2c:parser:state-type            = "long long"
   bnf2c:parser:top-state             = "top()"
   bnf2c:parser:pop-state             = "pop(<NB_STATES>);"
   bnf2c:parser:error-state           = "STATE_ERROR"
   bnf2c:parser:accept-state          = "STATE_ACCEPT"

   bnf2c:parser:value-type            = "T_VALUE"
   bnf2c:parser:push-value            = "valueStack[nbValues++] = (T_VALUE) <VALUE>;"
   bnf2c:parser:pop-values            = "nbValues -= <NB_VALUES>;"
   bnf2c:parser:get-value             = "valueStack[nbValues - <VALUE_IDX> - 1]"
   bnf2c:parser:value-as-token        = "<VALUE>.token"
   bnf2c:parser:value-as-intermediate = "<VALUE>.<TYPE>"

   bnf2c:lexer:token-type             = "T_TOKEN"
   bnf2c:lexer:shift-token            = "nextToken();"
   bnf2c:lexer:token-prefix           = ""
   bnf2c:lexer:end-of-input-token     = "EOI"

   bnf2c:output:intermediate-type     = "long long"
   bnf2c:output:parse-function        = "parseFunction"
   bnf2c:output:branch-function       = "branchFunction"

   bnf2c:generator:default-switch     = "true"
   bnf2c:generator:branch-table       = "true"

   bnf2c:indent:string                = "    "
   bnf2c:indent:top                   = "0"

   bnf2c:type<value> B E START
   bnf2c:type<token> TOKEN
*/

int top(void)
{
    return stateStack[currentState-1];
}

void pop(int nbStates)
{
    currentState -= nbStates;
}

void pushState(int state)
{
    stateStack[currentState++] = state;
}

void nextToken(void)
{
    for(;;)
    {
        /*!re2c
        re2c:define:YYCTYPE = "char";
        re2c:define:YYCURSOR = input;
        re2c:define:YYMARKER = marker;
        re2c:indent:top = 1;
        re2c:indent:string = "    ";
        re2c:yyfill:enable = 0;

        [ \t]+ { continue; }

        "*" { token = MULT; break; }
        "+" { token = ADD;  break; }
        "0" { token = ZERO; break; }
        "1" { token = ONE;  break; }

        "\000" { token = EOI;   break; }
        [^]    { token = ERROR; break; }
        */
    }
}

/*!bnf2c
<START> ::= <E>

<E> ::= <E> MULT <B>
        {
                /* printf("Multiply %ld with %ld\n", $1, $3); */
                $$ = $1 * $3;
        }
      | <E> ADD  <B>
        {
                /* printf("Add %ld with %ld\n", $1, $3); */
                $$ = $1 + $3;
        }
      | <B>
*/

/*!bnf2c
<B> ::= ZERO { $$ = 0; }
<B> ::= ONE  { $$ = 1; }
*/
