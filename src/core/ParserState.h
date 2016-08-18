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

#include <memory>
#include <unordered_set>
#include <list>
#include <ostream>

class Grammar;

class ParserState
{
    public :
        using Ptr = std::unique_ptr<ParserState>;
        using ItemList = std::list<Item>;

    public :
        bool contains(const Item & item) const;
        void assignSuccessors(const std::string & nextSymbol, const ParserState & nextState);
        virtual void close(const Grammar & grammar) = 0;
        virtual bool isMergeableWith(const Ptr & state) = 0;
        virtual void merge(Ptr & state) { /* By default, do nothing */ }

        void check(Errors<GeneratingError> & errors) const;

        void printDebugActions (std::ostream & os, const Grammar & grammar, const Options & options) const;
        void printDebugBranches(std::ostream & os, const Grammar & grammar, std::size_t size) const;

    public :
        ItemList items;
        int      numState;

    protected :
        bool symbolNeedsToBeClosed(const Symbol & symbol);

        SymbolSet symbolsAlreadyClosed;
};

std::ostream & operator <<(std::ostream & os, const Item & item);
std::ostream & operator <<(std::ostream & os, const ParserState & itemSet);

#endif /* _PARSERSTATE_H_ */
