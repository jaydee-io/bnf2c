////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _ITEM_H_
#define _ITEM_H_
#include "Symbol.h"

class Rule;
class ParserState;

struct Item
{
    enum class ActionType
    {
        SHIFT,
        REDUCE
    };


    const Rule & rule;
    SymbolList::const_iterator dottedSymbol;
    const ParserState * nextState;
    SymbolSet lookaheads;


    Item(void) = default;
    Item(const Rule & rule, SymbolList::const_iterator dot, const ParserState * nextState);
    Item(const Rule & rule, SymbolList::const_iterator dot, const ParserState * nextState, SymbolSet && lookahead);

    bool operator ==(const Item & item) const;
    bool operator < (const Item & item) const;

    Item::ActionType getType(void) const;
    bool isShift(void)  const { return getType() == ActionType::SHIFT; }
    bool isReduce(void) const { return getType() == ActionType::REDUCE; }

    bool isDotAtEnd(void) const;
    bool isNextSymbolEqualTo(const std::string & name);
};

#endif /* _ITEM_H_ */

