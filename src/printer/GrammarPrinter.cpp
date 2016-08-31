////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "PrettyPrinters.h"
#include "core/Grammar.h"

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Grammar & grammar)
{
    for(const auto & rulePair : grammar.rules)
    {
        os << "[" << rulePair.second.numRule << "] " << rulePair.second << std::endl ;
        os << '"' << rulePair.second.action << '"' << std::endl;
        os << std::endl;
    }

    return os;
}
 
