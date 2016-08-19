////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "LALR1State.h"

////////////////////////////////////////////////////////////////////////////////
bool LALR1State::isMergeableWith(const ParserState::Ptr & state)
{
    if(items.size() != state->items.size())
        return false;

    for(auto itemThis = items.begin(), itemState = state->items.begin(); itemThis != items.end(); ++itemThis, ++itemState)
    {
        if(itemThis->dottedSymbol != itemState->dottedSymbol ||
           itemThis->rule         != itemState->rule)
            return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void LALR1State::merge(Ptr & state)
{
    for(auto itemThis = items.begin(), itemState = state->items.begin(); itemThis != items.end(); ++itemThis, ++itemState)
        for(auto && lookahead : itemState->lookaheads)
            itemThis->lookaheads.insert(std::move(lookahead));
}

