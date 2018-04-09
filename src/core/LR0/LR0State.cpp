////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "LR0State.h"
#include "core/Rule.h"
#include "utils/Algos.h"

////////////////////////////////////////////////////////////////////////////////
void LR0State::addItem(const Rule & rule, SymbolList::const_iterator dottedSymbol)
{
    items.emplace_back(rule, dottedSymbol, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
void LR0State::addItemsRange(const Grammar::RuleRange & ruleRange)
{
    for_each(ruleRange, [this](const auto & rule)
    {
        this->addItem(rule.second, rule.second.symbols.begin()); // GCC 6.3 bug : need to explicitly use 'this->'
    });
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

    return all_of_pairs(items, state->items, [](auto & itemThis, auto & itemState)
    {
        return itemThis.dottedSymbol == itemState.dottedSymbol
            && itemThis.rule         == itemState.rule;
    });
}

////////////////////////////////////////////////////////////////////////////////
bool LR0State::symbolNeedsToBeClosed(const Symbol & symbol)
{
    if(symbol.isIntermediate() && symbolsAlreadyClosed.find(symbol) == symbolsAlreadyClosed.end())
    {
        symbolsAlreadyClosed.insert(symbol);
        return true;
    }

    return false;
}

