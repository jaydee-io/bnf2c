////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _LR0STATE_H_
#define _LR0STATE_H_
#include "core/ParserState.h"
#include "core/Grammar.h"

class Rule;

class LR0State : public ParserState
{
    public :
        void addItem(const Rule & rule, const SymbolIterator dot = 0);
        void close(const Grammar & grammar) override;

    private :
        void addItemsRange(const Grammar::RuleRange & ruleRange);
};

#endif /* _LR0STATE_H_ */
