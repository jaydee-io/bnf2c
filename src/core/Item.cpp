////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "ParserState.h"
#include "Rule.h"

#include <map>
#include <unordered_set>
#include <sstream>
#include <algorithm>
#include <iomanip>

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
bool Item::isNextSymbolEqualTo(const std::string & name)
{
    return !isDotAtEnd() && dottedSymbol->name == name;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Item & item)
{
    if(item.isReduce())
        os << "[R" << item.rule.numRule << "] ";
    else if(item.isShift())
        os << "[S" << (item.nextState != nullptr ? item.nextState->numState : -1) << "] ";

    os << "<" << item.rule.name << "> ::=";

    for(auto symbolIt = item.rule.symbols.begin(); symbolIt != item.rule.symbols.end(); ++symbolIt)
    {
        if(symbolIt == item.dottedSymbol)
            os << " •" << *symbolIt;
        else
            os << " " << *symbolIt;
    }

    if(item.dottedSymbol == item.rule.symbols.end())
        os << " •";

    if(!item.lookaheads.empty())
    {
        os << ", ";
        for(auto itSymbol = item.lookaheads.begin(); itSymbol != item.lookaheads.end(); ++itSymbol)
        {
            os << *itSymbol;

            auto nextSymbol = itSymbol;
            ++nextSymbol;
            if(nextSymbol != item.lookaheads.end())
                os << '/';
        }
    }

    return os;
}
