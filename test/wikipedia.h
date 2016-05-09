#ifdef __cplusplus
extern "C" {
#endif


/* Lexer */
typedef enum {
    MULT,
    ADD,
    ZERO,
    ONE,
    EOI,
    ERROR
} T_TOKEN;

extern const char * input;
extern T_TOKEN token;

extern void nextToken(void);

/* Parser */


#define NB_STATES 256
#define STATE_ERROR  -5
#define STATE_ACCEPT -6
extern long long stateStack[NB_STATES];
extern int currentState;

typedef union {
    long    value;
    T_TOKEN token;
} T_VALUE;

extern T_VALUE valueStack[NB_STATES];
extern int nbValues;

extern int top(void);
extern void pop(int nbStates);
extern void pushState(int state);
extern long long parseFunction(T_TOKEN);


#ifdef __cplusplus
}
#endif
