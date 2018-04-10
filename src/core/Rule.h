////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef RULE_H
#define RULE_H
#include "Symbol.h"

#include <string>

class Rule
{
    public :
        Rule(void) = default;
        Rule(const std::string & name);

        void addSymbol(const Symbol & symbol);
        void addSymbol(Symbol && symbol);

        SymbolList remainingSymbolsAfter(SymbolList::const_iterator symbol) const;

        bool operator ==(const Rule & rule) const;
        bool operator !=(const Rule & rule) const;

    public :
        std::string name;
        SymbolList  symbols;
        std::string action;
        int         numRule = -1;
};

#endif /* RULE_H */
