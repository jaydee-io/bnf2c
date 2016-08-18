////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _LR1STATE_H_
#define _LR1STATE_H_
#include "core/ParserState.h"
#include "core/Grammar.h"

class Rule;

class LR1State : public ParserState
{
    public :
        void addItem(const Rule & rule, const SymbolList::const_iterator dottedSymbol, SymbolSet && lookahead);
        void close(const Grammar & grammar) override;
        bool isMergeableWith(const Ptr & state) override;

    private :
        void addItemsRange(const Grammar::RuleRange & ruleRange, SymbolSet && lookahead);
};

#endif /* _LR1STATE_H_ */
