////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "PrettyPrinters.h"
#include "core/Symbol.h"

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Symbol & symbol)
{
    switch(symbol.type)
    {
        case Symbol::Type::TERMINAL     : os        << symbol.name;        break;
        case Symbol::Type::INTERMEDIATE : os << "<" << symbol.name << ">"; break;
    }

    return os;
}

