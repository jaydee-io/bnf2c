////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef ITEM_H
#define ITEM_H
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
    ParserState * nextState;
    SymbolSet lookaheads;


    Item(void) = default;
    Item(const Rule & rule, SymbolList::const_iterator dot, ParserState * nextState);
    Item(const Rule & rule, SymbolList::const_iterator dot, ParserState * nextState, SymbolSet && lookahead);

    bool operator ==(const Item & item) const;
    bool operator < (const Item & item) const;

    Item::ActionType getType(void) const;
    bool isShift(void)  const { return getType() == ActionType::SHIFT; }
    bool isReduce(void) const { return getType() == ActionType::REDUCE; }

    bool isDotAtEnd(void) const;
    bool isNextSymbolEqualTo(const std::string & name) const;
    bool isTerminalInLookaheads(const std::string & terminal) const;
};

#endif /* ITEM_H */

