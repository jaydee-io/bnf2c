////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "wikipedia.h"
#include "gtest/gtest.h"

long long stateStack[NB_STATES];
int currentState;

T_VALUE valueStack[NB_STATES];
int nbValues;

const char * input;

T_TOKEN token = EOI;

TEST(Wikipedia, AddAndMultiply)
{
    input = "1+0+1*1+1+1+1+1+1*1";

    nextToken();
    pushState(0);
    while((top() != STATE_ERROR) && (top() != STATE_ACCEPT))
        pushState(parseFunction(token));

    EXPECT_EQ(STATE_ACCEPT, top()) << "An error has occured while parsing expression";
    EXPECT_EQ(7, valueStack[nbValues - 1].value);
}

