////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "PrettyPrinters.h"
#include "core/Grammar.h"
#include <set>

////////////////////////////////////////////////////////////////////////////////
struct RulePointerLessThan
{
    bool operator()(const Rule * lhs, const Rule * rhs) const
    {
        return lhs->numRule < rhs->numRule;
    }
};

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Grammar & grammar)
{
    // Sort rules by number
    std::set<const Rule *, RulePointerLessThan> rules;
    for(const auto & rulePair : grammar.rules)
        rules.insert(&rulePair.second);

    // Print each rule
    for(const auto rulePtr : rules)
    {
        os << "[" << rulePtr->numRule << "] " << *rulePtr << std::endl ;
        os << '"' << rulePtr->action << '"' << std::endl;
        os << std::endl;
    }

    return os;
}
 
