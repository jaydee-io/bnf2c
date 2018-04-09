////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "LALR1State.h"
#include "utils/Algos.h"

////////////////////////////////////////////////////////////////////////////////
bool LALR1State::isMergeableWith(const ParserState::Ptr & state)
{
    if(items.size() != state->items.size())
        return false;

    return all_of_pairs(items, state->items, [](auto & itemThis, auto & itemState)
    {
            return itemThis.dottedSymbol == itemState.dottedSymbol
                && itemThis.rule         == itemState.rule;
        });
}

////////////////////////////////////////////////////////////////////////////////
void LALR1State::merge(Ptr & state)
{
    for_each_pair(items, state->items, [](auto & itemThis, auto & itemState)
    {
        for(auto && lookahead : itemState.lookaheads)
            itemThis.lookaheads.insert(std::move(lookahead));
    });
}

