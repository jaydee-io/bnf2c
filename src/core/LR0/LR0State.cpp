////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "LR0State.h"
#include "core/Rule.h"

////////////////////////////////////////////////////////////////////////////////
void LR0State::addItem(const Rule & rule, const SymbolIterator dot)
{
    items.emplace_back(rule, dot, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
void LR0State::addItemsRange(const Grammar::RuleRange & ruleRange)
{
    for(Grammar::RuleIterator ruleIt = ruleRange.first; ruleIt != ruleRange.second; ++ruleIt)
        addItem(ruleIt->second);
}

////////////////////////////////////////////////////////////////////////////////
void LR0State::close(const Grammar & grammar)
{
    for(auto & item : items)
    {
        if(item.dot == item.rule.symbols.size())
            continue;

        const Symbol & symbol = item.getDottedSymbol();

        if(symbolNeedsToBeClosed(symbol))
            addItemsRange(grammar[symbol.name]);
    }
}

