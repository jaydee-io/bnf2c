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

#include <memory>
#include <list>

class Grammar;

class ParserState
{
    public :
        using Ptr = std::unique_ptr<ParserState>;
        using ItemList = std::list<Item>;

        struct Action
        {
            enum class Type
            {
                SHIFT,
                REDUCE,
                ACCEPT,
                ERROR
            } type;

            union 
            {
                const Rule * reduceRule;
                const ParserState * shiftNextState;
            };
        };

    public :
        bool contains(const Item & item) const;
        void assignSuccessors(const std::string & nextSymbol, const ParserState & nextState);
        virtual void close(const Grammar & grammar) = 0;
        virtual bool isMergeableWith(const Ptr & state) = 0;
        virtual void merge(Ptr & state) { /* By default, do nothing */ }

        void check(Errors<GeneratingError> & errors) const;

        Action getAction(const std::string & terminal, const std::string & endOfInputToken) const;
        const ParserState * getGoto(const std::string & intermediate) const;

    public :
        ItemList items;
        int      numState;

    protected :
        bool symbolNeedsToBeClosed(const Symbol & symbol);

        SymbolSet symbolsAlreadyClosed;
};

#endif /* _PARSERSTATE_H_ */
