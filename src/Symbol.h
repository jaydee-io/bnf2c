////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef __SYMBOL_H__
#define __SYMBOL_H__
#include "Dictionary.h"

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

    bool operator ==(const Symbol & symbol) const;
};

typedef std::vector<Symbol> SymbolList;
typedef std::size_t         SymbolIterator;

std::ostream & operator <<(std::ostream & os, const Symbol & symbol);

#endif /* __SYMBOL_H__ */
