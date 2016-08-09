////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _PARSERSTATE_H_
#define _PARSERSTATE_H_
#include "Item.h"
#include "Grammar.h"
#include "config/Options.h"
#include "Errors.h"

#include <unordered_set>
#include <list>
#include <ostream>

class Rule;

class ParserState
{
    public :
        typedef std::list<Item> ItemList;

    public :
        void addRule(const Rule & rule, const SymbolIterator dot = 0);
        bool contains(const Item & item) const;
        void close(const Grammar & grammar);
        void assignSuccessors(const std::string & nextSymbol, const ParserState & nextState);

        void check(Errors<GeneratingError> & errors) const;

        void printDebugActions (std::ostream & os, const Grammar & grammar, const Options & options) const;
        void printDebugBranches(std::ostream & os, const Grammar & grammar, std::size_t size) const;

        bool operator ==(const ParserState & set) const;

    public :
        ItemList items;
        int      numState;

    private :
        void addRules(const Grammar::RuleRange & ruleRange);
        bool symbolNeedsToBeClosed(const Symbol & symbol);

        std::unordered_set<std::string>  symbolsAlreadyClosed;
};

std::ostream & operator <<(std::ostream & os, const Item & item);
std::ostream & operator <<(std::ostream & os, const ParserState & itemSet);

#endif /* _PARSERSTATE_H_ */
