////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "ParserState.h"
#include "Rule.h"

#include <map>

////////////////////////////////////////////////////////////////////////////////
Item::Item(const Rule & rule, SymbolList::const_iterator dottedSymbol, const ParserState * nextState)
: rule(rule), dottedSymbol(dottedSymbol), nextState(nextState)
{
}

////////////////////////////////////////////////////////////////////////////////
Item::Item(const Rule & rule, SymbolList::const_iterator dottedSymbol, const ParserState * nextState, SymbolSet && lookaheads)
: rule(rule), dottedSymbol(dottedSymbol), nextState(nextState), lookaheads(std::forward<SymbolSet>(lookaheads))
{
}

////////////////////////////////////////////////////////////////////////////////
bool Item::operator ==(const Item & item) const
{
    if(&item == this)
        return true;

    return dottedSymbol == item.dottedSymbol &&
           rule == item.rule &&
           lookaheads == item.lookaheads;
}

////////////////////////////////////////////////////////////////////////////////
bool Item::operator <(const Item & item) const
{
    if(&item == this)
        return false;

    if(rule.numRule != item.rule.numRule)
        return (rule.numRule < item.rule.numRule);
    else
        return (dottedSymbol < item.dottedSymbol);
}

////////////////////////////////////////////////////////////////////////////////
Item::ActionType Item::getType(void) const
{
    if(isDotAtEnd())
        return ActionType::REDUCE;
    else
        return ActionType::SHIFT;
}

////////////////////////////////////////////////////////////////////////////////
bool Item::isDotAtEnd(void) const
{
    return dottedSymbol == rule.symbols.end();
}

////////////////////////////////////////////////////////////////////////////////
bool Item::isNextSymbolEqualTo(const std::string & name) const
{
    return !isDotAtEnd() && dottedSymbol->name == name;
}

////////////////////////////////////////////////////////////////////////////////
bool Item::isTerminalInLookaheads(const std::string & terminal) const
{
    return lookaheads.empty() || lookaheads.find({ Symbol::Type::TERMINAL, terminal }) != lookaheads.end();
}
