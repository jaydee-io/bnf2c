////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _PARSERSTATE_H_
#define _PARSERSTATE_H_
#include "Rule.h"
#include "Grammar.h"
#include "Options.h"
#include "Errors.h"

#include <list>
#include <ostream>
#include <string>

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

    bool operator ==(const Item & item) const;
    bool operator < (const Item & item) const;
    Item::ActionType getType(void) const;
};

class ParserState
{
    public :
        typedef std::list<Item> ItemList;

    public :
        void addRule(const Rule & rule, const SymbolIterator dot = 0);
        bool contains(const Item & item) const;
        void close(const Grammar & grammar);

        void check(Errors<GeneratingError> & errors) const;

        void generateActions       (std::ostream & os, Options & options, const Grammar & grammar) const;
        void generateBranchesSwitch(std::ostream & os, Options & options, const Grammar & grammar) const;
        void generateBranchesTable (std::ostream & os, Options & options, const Grammar & grammar) const;

        void printDebugActions (std::ostream & os, const Grammar & grammar, const Options & options) const;
        void printDebugBranches(std::ostream & os, const Grammar & grammar, std::size_t size) const;

        bool operator ==(const ParserState & set) const;

    protected :
        void generateActionItems (std::ostream & os, Options & options, const Grammar & grammar) const;
        void generateReduceAction(const Item & item, std::ostream & os, Options & options, const Grammar & grammar) const;
        void generateShiftAction (const Item & item, std::ostream & os, Options & options, const Grammar & grammar) const;

        std::string checkedStringReplace(const std::string & str, const std::string & pattern, const std::string & replacement) const;

    public :
        ItemList items;
        int      numState;

    protected :
        bool    m_isClosed = false;
        Item *  m_reduceRule = nullptr;
        bool    m_isAnAcceptRule = false;
};

std::ostream & operator <<(std::ostream & os, const Item & item);
std::ostream & operator <<(std::ostream & os, const ParserState & itemSet);

#endif /* _PARSERSTATE_H_ */
