#include <stdio.h>

/*!bnf2c
   bnf2c:parser:state-type        = "long long"
   bnf2c:parser:top-state         = "top()"
   bnf2c:parser:pop-state         = "pop(<NB_STATES>);"
   bnf2c:parser:error-state       = "STATE_ERROR"
   bnf2c:parser:accept-state      = "STATE_ACCEPT"

   bnf2c:parser:value-type        = "T_VALUE"
   bnf2c:parser:push-value        = "valueStack[nbValues++] = <VALUE>;"
   bnf2c:parser:pop-values        = "nbValues -= <NB_VALUES>;"
   bnf2c:parser:get-value         = "valueStack[nbValues - <VALUE_IDX> - 1]"

   bnf2c:lexer:token-type         = "T_TOKEN"
   bnf2c:lexer:token-union-name   = "token"
   bnf2c:lexer:shift-token        = "nextToken();"
   bnf2c:lexer:token-prefix       = ""
   bnf2c:lexer:end-of-input-token = "EOI"

   bnf2c:output:intermediate-type = "long long"
   bnf2c:output:parse-function    = "parseFunction"
   bnf2c:output:branch-function   = "branchFunction"

   bnf2c:generator:default-switch = "true"
   bnf2c:generator:branch-table   = "true"

   bnf2c:indent:string            = "    "
   bnf2c:indent:top               = "0"

   bnf2c:type<value> B E START
   bnf2c:type<token> TOKEN
*/

typedef enum {
    MULT,
    ADD,
    ZERO,
    ONE,
    EOI,
    ERROR
} T_TOKEN;

long long parseFunction(T_TOKEN);


#define NB_STATES 256
#define STATE_ERROR  -5
#define STATE_ACCEPT -6

long long stateStack[NB_STATES];
int currentState;

typedef union {
    long    value;
    T_TOKEN token;
} T_VALUE;

T_VALUE valueStack[NB_STATES];
int nbValues;

char * input;

T_TOKEN token = EOI;



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
                // printf("Multiply %ld with %ld\n", $1, $3);
                $$ = $1 * $3;
        }
      | <E> ADD  <B>
        {
                // printf("Add %ld with %ld\n", $1, $3);
                $$ = $1 + $3;
        }
      | <B>
*/

/*!bnf2c
<B> ::= ZERO { $$ = 0; }
<B> ::= ONE  { $$ = 1; }
*/

int main(int argc, char ** argv)
{
    if(argc < 2)
        return 1;

    input = argv[1];
    nextToken();

    pushState(0);
    while((top() != STATE_ERROR) && (top() != STATE_ACCEPT))
        pushState(parseFunction(token));

    if(top() == STATE_ERROR)
        printf("Error\n");
    else
        printf("The result of \"%s\" = %ld\n", argv[1], valueStack[nbValues - 1].value);

    return 0;
}
