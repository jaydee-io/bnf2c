////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "LR0State.h"
#include "core/Rule.h"

////////////////////////////////////////////////////////////////////////////////
void LR0State::addItem(const Rule & rule, SymbolList::const_iterator dottedSymbol)
{
    items.emplace_back(rule, dottedSymbol, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
void LR0State::addItemsRange(const Grammar::RuleRange & ruleRange)
{
    for(Grammar::RuleIterator ruleIt = ruleRange.first; ruleIt != ruleRange.second; ++ruleIt)
        addItem(ruleIt->second, ruleIt->second.symbols.begin());
}

////////////////////////////////////////////////////////////////////////////////
void LR0State::close(const Grammar & grammar)
{
    for(auto & item : items)
    {
        if(item.isDotAtEnd())
            continue;

        const Symbol & symbol = *item.dottedSymbol;

        if(symbolNeedsToBeClosed(symbol))
            addItemsRange(grammar[symbol.name]);
    }
}

////////////////////////////////////////////////////////////////////////////////
bool LR0State::isMergeableWith(const ParserState::Ptr & state)
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
