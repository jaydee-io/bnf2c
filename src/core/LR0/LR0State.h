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
        void addItem(const Rule & rule, SymbolList::const_iterator dottedSymbol);
        void close(const Grammar & grammar) override;
        bool isMergeableWith(const Ptr & state) override;

    private :
        void addItemsRange(const Grammar::RuleRange & ruleRange);
        bool symbolNeedsToBeClosed(const Symbol & symbol);

        SymbolSet symbolsAlreadyClosed;
};

#endif /* _LR0STATE_H_ */
