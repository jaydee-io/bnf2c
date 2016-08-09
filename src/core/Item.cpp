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
Item::Item(const Rule & rule, SymbolIterator dot, const ParserState * nextState)
: rule(rule), dot(dot), nextState(nextState)
{
}

////////////////////////////////////////////////////////////////////////////////
bool Item::operator ==(const Item & item) const
{
    if(&item == this)
        return true;

    return (dot == item.dot) && (rule == item.rule);
}

////////////////////////////////////////////////////////////////////////////////
bool Item::operator <(const Item & item) const
{
    if(&item == this)
        return false;

    if(rule.numRule != item.rule.numRule)
        return (rule.numRule < item.rule.numRule);
    else
        return (dot < item.dot);
}

////////////////////////////////////////////////////////////////////////////////
Item::ActionType Item::getType(void) const
{
    if(dot >= rule.symbols.size())
        return ActionType::REDUCE;
    else
        return ActionType::SHIFT;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Item & item)
{
    if(item.getType() == Item::ActionType::REDUCE)
        os << "[R" << item.rule.numRule << "] ";
    else if(item.getType() == Item::ActionType::SHIFT)
        os << "[S" << (item.nextState != nullptr ? item.nextState->numState : -1) << "] ";

    os << "<" << item.rule.name << "> ::=";

    for(SymbolIterator it = 0; it < item.rule.symbols.size(); ++it)
    {
        if(it == item.dot)
            os << " •" << item.rule.symbols[it];
        else
            os << " " << item.rule.symbols[it];
    }

    if(item.dot == item.rule.symbols.size())
        os << " •";

    return os;
}

