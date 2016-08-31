////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "PrettyPrinters.h"
#include "core/Rule.h"

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Rule & rule)
{
    os << "<" << rule.name << "> ::=";

    for(SymbolList::const_iterator it = rule.symbols.begin(); it != rule.symbols.end(); ++it)
        os << " " << *it;

    return os;
}
