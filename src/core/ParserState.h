////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _PARSERSTATE_H_
#define _PARSERSTATE_H_
#include "Item.h"
#include "Errors.h"
#include "ParsingAction.h"

#include <memory>
#include <list>

class Grammar;

class ParserState
{
    public :
        using Ptr = std::unique_ptr<ParserState>;
        using ItemList = std::list<Item>;

    public :
        virtual ~ParserState(void) = default;

        bool contains(const Item & item) const;
        void assignSuccessors(const std::string & nextSymbol, ParserState & nextState);
        virtual void close(const Grammar & grammar) = 0;
        virtual bool isMergeableWith(const Ptr & state) = 0;
        virtual void merge(Ptr & state) { /* By default, do nothing */ }

        void check(Errors<GeneratingError> & errors) const;

        ParsingAction getAction(const std::string & terminal, const std::string & endOfInputToken) const;
        const ParserState * getGoto(const std::string & intermediate) const;

        bool isSameActionForAllTerminals(const Grammar & grammar, const std::string & endOfInputToken) const;

    public :
        ItemList items;
        int      numState;
};

#endif /* _PARSERSTATE_H_ */
