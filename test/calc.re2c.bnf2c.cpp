#include <iostream>
#include <stack>
#include <deque>

/*!bnf2c
   bnf2c:parser:top-state         = "stateStack.top()"
   bnf2c:parser:pop-state         = "for(int i=0; i<<NB_STATES>; i++) stateStack.pop();"
   bnf2c:parser:error-state       = "STATE_ERROR"
   bnf2c:parser:accept-state      = "STATE_ACCEPT"

   bnf2c:parser:value-type        = "Value"
   bnf2c:parser:push-value        = "push_value(<VALUE>);"
   bnf2c:parser:pop-values        = "pop_values(<NB_VALUES>);"
   bnf2c:parser:get-value         = "get_value(<VALUE_IDX>)"

   bnf2c:lexer:token-type         = "Token"
   bnf2c:lexer:shift-token        = "nextToken();"
   bnf2c:lexer:token-prefix       = ""
   bnf2c:lexer:get-type-of-token  = "<TOKEN>.type"
   bnf2c:lexer:end-of-input-token = "EOI"

   bnf2c:output:intermediate-type = "long long"
   bnf2c:output:parse-function    = "parseFunction"
   bnf2c:output:branch-function   = "branchFunction"

   bnf2c:generator:default-switch = "false"
   bnf2c:generator:branch-table   = "true"

   bnf2c:indent:string            = "    "
   bnf2c:indent:top               = "0"

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
    char *  start;

    long long number(void) const { return ::atoll(start); }
};

union Value
{
    long long   value;
    Token       token;
};

int parseFunction(Token);

#define STATE_ERROR  -5
#define STATE_ACCEPT -6

std::stack<int>     stateStack;
std::deque<Value>   valueStack;

char * input;

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

void push_value(Value & value)
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

int main(int argc, char ** argv)
{
    if(argc < 2)
        return 1;

    input = argv[1];
    nextToken();

    stateStack.push(0);
    while((stateStack.top() != STATE_ERROR) && (stateStack.top() != STATE_ACCEPT))
        stateStack.push(parseFunction(token));

    if(stateStack.top() == STATE_ERROR)
        std::cout << "Error" << std::endl;
    else
        std::cout << "The result of \"" << argv[1] << "\" = " << valueStack.back().value << std::endl;

    return 0;
}
