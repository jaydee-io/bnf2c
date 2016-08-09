////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _PARSERSTATE_H_
#define _PARSERSTATE_H_
#include "Item.h"
#include "config/Options.h"
#include "Errors.h"

#include <list>
#include <ostream>

class Rule;
class Grammar;

class ParserState
{
    public :
        typedef std::list<Item> ItemList;

    public :
        void addRule(const Rule & rule, const SymbolIterator dot = 0);
        bool contains(const Item & item) const;
        void close(const Grammar & grammar);

        void check(Errors<GeneratingError> & errors) const;

        bool isAnAcceptRule(void) const;
        Item *  getReduceRule(void) const;

        void printDebugActions (std::ostream & os, const Grammar & grammar, const Options & options) const;
        void printDebugBranches(std::ostream & os, const Grammar & grammar, std::size_t size) const;

        bool operator ==(const ParserState & set) const;

    public :
        ItemList items;
        int      numState;

    protected :
        Item *  m_reduceRule = nullptr;
        bool    m_isAnAcceptRule = false;
};

std::ostream & operator <<(std::ostream & os, const Item & item);
std::ostream & operator <<(std::ostream & os, const ParserState & itemSet);

#endif /* _PARSERSTATE_H_ */
