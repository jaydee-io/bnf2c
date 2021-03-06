////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "LR1State.h"
#include "core/Rule.h"
#include "utils/Algos.h"

////////////////////////////////////////////////////////////////////////////////
void LR1State::addItem(const Rule & rule, const SymbolList::const_iterator dottedSymbol, SymbolSet && lookahead)
{
    // If the item already exist, merge the lookaheads
    for(auto & item : items)
    {
        if(dottedSymbol == item.dottedSymbol && rule == item.rule)
        {
            item.lookaheads.insert(lookahead.begin(), lookahead.end());
            return;
        }
    }

    // Add a new item
    items.emplace_back(rule, dottedSymbol, nullptr, std::forward<SymbolSet>(lookahead));
}

////////////////////////////////////////////////////////////////////////////////
void LR1State::addItemsRange(const Grammar::RuleRange & ruleRange, SymbolSet && lookahead)
{
    for_each(ruleRange, [&](const auto & rule)
    {
        this->addItem(rule.second, rule.second.symbols.begin(), SymbolSet(lookahead)); // GCC 6.3 bug : need to explicitly use 'this->'
    });
}

////////////////////////////////////////////////////////////////////////////////
SymbolSet allLookaheadsOf(const Item & item, const Grammar & grammar)
{
    SymbolList followingSymbols = item.rule.remainingSymbolsAfter(item.dottedSymbol);
    followingSymbols.push_back({ Symbol::Type::TERMINAL, "" });

    SymbolSet allLookaheads;
    for(auto & lookahead : item.lookaheads)
    {
        followingSymbols.back() = lookahead;
        auto first = grammar.first(followingSymbols);
        allLookaheads.insert(first.begin(), first.end());
    }

    return allLookaheads;
}

////////////////////////////////////////////////////////////////////////////////
void LR1State::close(const Grammar & grammar)
{
    for(auto & item : items)
    {
        if(item.isDotAtEnd() || item.dottedSymbol->isTerminal())
            continue;

        // Current item is of the form 'A –> u•Bv, x/y/z' (With dottedSymbol = B and lookaheads = x/y/z)
        // We need to add each B production rule which have a lookahead 'v' followed by ether 'x', 'y' or 'z'
        // This lookahead is the concatenation of FIRST(vx), FIRST(vx) and FIRST(vx)
        addItemsRange(grammar[item.dottedSymbol->name], allLookaheadsOf(item, grammar));
    }
}

////////////////////////////////////////////////////////////////////////////////
bool LR1State::isMergeableWith(const ParserState::Ptr & state)
{
    if(items.size() != state->items.size())
        return false;

    return all_of_pairs(items, state->items, [](auto & itemThis, auto & itemState)
    {
        return itemThis.dottedSymbol == itemState.dottedSymbol
            && itemThis.rule         == itemState.rule
            && itemThis.lookaheads   == itemState.lookaheads;
    });
}

