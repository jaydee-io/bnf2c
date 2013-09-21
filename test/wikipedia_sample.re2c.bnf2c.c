#include <stdio.h>

/*!bnf2c
   bnf2c:state:top               = "top()"
   bnf2c:state:pop               = "pop(<NB_STATES>)"
   bnf2c:state:error             = "STATE_ERROR"
   bnf2c:state:accept            = "STATE_ACCEPT"

   bnf2c:state:pushValue         = "pushValue((void *) <VALUE>)"
   bnf2c:state:popValues         = "popValues(<NB_VALUES>)"
   bnf2c:state:getValue          = "getValue(<VALUE_IDX>)"

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
    MULT  = '*',
    ADD   = '+',
    ZERO  = '0',
    ONE   = '1',
    EOI   = '.',
    ERROR = 'E'
} T_TOKEN;



#define NB_STATES 256
#define STATE_ERROR  -5
#define STATE_ACCEPT -6

int stateStack[NB_STATES];
int currentState;

void * valueStack[NB_STATES];
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

void pushValue(void * value)
{
    valueStack[nbValues++] = value;
}

void popValues(int nbValuesToPop)
{
    nbValues -= nbValuesToPop;
}

void * getValue(int valueIdx)
{
    return valueStack[nbValues - valueIdx - 1];
}

void nextToken(void)
{
    if(token != EOI)
        pushValue((void *) token);

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
                // printf("Multiply %d with %d\n", $1, $3);
                $$ = (void *) ((long) $1 * (long) $3);
        }
      | <E> ADD  <B>
        {
                // printf("Add %d with %d\n", $1, $3);
                $$ = (void *) ((long) $1 + (long) $3);
        }
      | <B>
*/

/*!bnf2c
<B> ::= ZERO { $$ = (void *) 0; }
<B> ::= ONE  { $$ = (void *) 1; }
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
        printf("The result of \"%s\" = %ld\n", argv[1], (long)valueStack[nbValues - 1]);

    return 0;
}
