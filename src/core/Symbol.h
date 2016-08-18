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
#include <unordered_set>

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
    bool operator !=(const Symbol & symbol) const;
};

using SymbolList = std::vector<Symbol>;
using SymbolSet = std::unordered_set<Symbol>;

std::ostream & operator <<(std::ostream & os, const Symbol & symbol);

namespace std {

template <> 
struct hash<Symbol>
{
    size_t operator()(const Symbol & symbol) const
    {
        return (size_t) symbol.type + std::hash<std::string>()(symbol.name);
    }
};

}

#endif /* _SYMBOL_H_ */
