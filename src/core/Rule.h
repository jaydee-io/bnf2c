////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _RULE_H_
#define _RULE_H_
#include "Symbol.h"
#include "Dictionary.h"

#include <ostream>
#include <string>

class Rule
{
    public :
        Rule(void) = default;
        Rule(const std::string & name);

        void addSymbol(const Symbol & symbol);
        void addSymbol(Symbol && symbol);

        bool operator ==(const Rule & rule) const;

    public :
        std::string name;
        SymbolList  symbols;
        std::string action;
        int         numRule = -1;
};

std::ostream & operator <<(std::ostream & os, const Rule & rule);

#endif /* _RULE_H_ */