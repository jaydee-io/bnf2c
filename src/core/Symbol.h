////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _SYMBOL_H_
#define _SYMBOL_H_
#include <vector>
#include <iostream>
#include <string>

struct Symbol
{
    enum class Type
    {
        TERMINAL,
        INTERMEDIATE
    };

    Type        type;
    std::string name;

    bool isTerminal(void)     const { return type == Type::TERMINAL; }
    bool isIntermediate(void) const { return type == Type::INTERMEDIATE; }

    bool operator ==(const Symbol & symbol) const;
};

typedef std::vector<Symbol> SymbolList;
typedef std::size_t         SymbolIterator;

std::ostream & operator <<(std::ostream & os, const Symbol & symbol);

#endif /* _SYMBOL_H_ */
