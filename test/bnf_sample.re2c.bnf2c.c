#include "stdio.h"

/*!bnf2c
   bnf2c:state:top               = "top()"
   bnf2c:state:pop               = "pop(<NB_STATES>)"
   bnf2c:state:error             = "STATE_ERROR"
   bnf2c:state:accept            = "STATE_ACCEPT"

   bnf2c:type:state              = "long long"
   bnf2c:type:token              = "long long"
   bnf2c:type:intermediate       = "long long"

   bnf2c:token:shift             = "nextToken()"
   bnf2c:token:prefix            = ""
   bnf2c:token:endOfInput        = "EOI"


   bnf2c:generator:defaultSwitch = "false"
   bnf2c:generator:branchTable   = "true"

   bnf2c:output:parseFunction    = "parseFunction"
   bnf2c:output:branchFunction   = "branchFunction"

   bnf2c:indent:string           = "    "
   bnf2c:indent:top              = "0"
*/

long long parseFunction(long long token);


typedef enum {
    MULT,
    ADD,
    ZERO,
    ONE,
    EOI,
    ERROR
} T_TOKEN;



#define NB_STATES 256
#define STATE_ERROR  -5
#define STATE_ACCEPT -6

int stateStack[NB_STATES];
int currentState;

int valueStack[NB_STATES];
int currentValue;

char * input;

T_TOKEN token;




void nextToken(void)
{
    token = ERROR;

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

/*!bnf2c
<START> ::= <E>

<E> ::= <E> MULT <B>
        {
            // printf("Multiply %d with %d\n", valueStack[currentValue - 2], valueStack[currentValue - 1]);
            valueStack[currentValue - 2] = valueStack[currentValue - 2] * valueStack[currentValue - 1];
            currentValue--;
        }
      | <E> ADD  <B>
        {
            /* printf("Add %d with %d\n", valueStack[currentValue - 2], valueStack[currentValue - 1]); */
            valueStack[currentValue - 2] = valueStack[currentValue - 2] + valueStack[currentValue - 1];
            currentValue--;
        }
      | <B>
*/

/*!bnf2c
<B> ::= ZERO { valueStack[currentValue++] = 0; }
<B> ::= ONE  { valueStack[currentValue++] = 1; }
*/

int main(int argc, char ** argv)
{
    int state = 0;

    if(argc < 2)
        return 1;

    input = argv[1];
    nextToken();

    while((state != STATE_ERROR) && (state != STATE_ACCEPT))
    {
        pushState(state);
        state = parseFunction(token);
    }

    if(state == STATE_ERROR)
        printf("Error\n");
    else
        printf("The result of \"%s\" = %d\n", argv[1], valueStack[currentValue - 1]);

    return 0;
}
