////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "LR1State.h"
#include "core/Rule.h"

////////////////////////////////////////////////////////////////////////////////
void LR1State::addItem(const Rule & rule, const SymbolList::const_iterator dottedSymbol, SymbolSet && lookahead)
{
    items.emplace_back(rule, dottedSymbol, nullptr, std::forward<SymbolSet>(lookahead));
}

////////////////////////////////////////////////////////////////////////////////
void LR1State::addItemsRange(const Grammar::RuleRange & ruleRange, SymbolSet && lookahead)
{
    for(Grammar::RuleIterator ruleIt = ruleRange.first; ruleIt != ruleRange.second; ++ruleIt)
        addItem(ruleIt->second, ruleIt->second.symbols.begin(), SymbolSet(lookahead));
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
        if(item.isDotAtEnd())
            continue;

        const Symbol & symbol = *item.dottedSymbol;

        // Current item is of the form 'A –> u•Bv, x/y/z' (With dottedSymbol = B and lookaheads = x/y/z)
        // We need to add each B production rule which have a lookahead 'v' followed by ether 'x', 'y' or 'z'
        // This lookahead is the concatenation of FIRST(va), FIRST(vb) and FIRST(vb)
        if(symbolNeedsToBeClosed(symbol))
            addItemsRange(grammar[symbol.name], allLookaheadsOf(item, grammar));
    }
}

////////////////////////////////////////////////////////////////////////////////
bool LR1State::isMergeableWith(const ParserState::Ptr & state)
{
    if(items.size() != state->items.size())
        return false;

    for(auto itemThis = items.begin(), itemState = state->items.begin(); itemThis != items.end(); ++itemThis, ++itemState)
    {
        if(itemThis->dottedSymbol != itemState->dottedSymbol ||
           itemThis->rule         != itemState->rule ||
           itemThis->lookaheads   != itemState->lookaheads)
            return false;
    }

    return true;
}

