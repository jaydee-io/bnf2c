////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "PrettyPrinters.h"
#include "core/Item.h"
#include "core/Rule.h"
#include "core/ParserState.h"

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Item & item)
{
    if(item.isReduce())
        os << "[R" << item.rule.numRule << "] ";
    else if(item.isShift())
        os << "[S" << (item.nextState != nullptr ? item.nextState->numState : -1) << "] ";

    os << "<" << item.rule.name << "> ::=";

    // We need an iterator to compare to the dotted symbol
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
        os << ", " << separate_elems(item.lookaheads, "/");

    return os;
}
