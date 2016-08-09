////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _ITEM_H_
#define _ITEM_H_
#include "Symbol.h"

#include <ostream>

class Rule;
class ParserState;

struct Item
{
    enum class ActionType
    {
        SHIFT,
        REDUCE
    };

    const Rule &        rule;
    SymbolIterator      dot;
    const ParserState * nextState;

    Item(void) = default;
    Item(const Rule & rule, SymbolIterator dot, const ParserState * nextState);

    bool operator ==(const Item & item) const;
    bool operator < (const Item & item) const;

    Item::ActionType getType(void) const;
    bool isShift(void)  const { return getType() == ActionType::SHIFT; }
    bool isReduce(void) const { return getType() == ActionType::REDUCE; }

    const Symbol & getDottedSymbol(void) const;
    bool isNextSymbolEqualTo(const std::string & name);
};

std::ostream & operator <<(std::ostream & os, const Item & item);

#endif /* _ITEM_H_ */

