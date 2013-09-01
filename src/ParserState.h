////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013, Jerome DUMESNIL
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by Jerome DUMESNIL.
// 4. Neither the name of the Jerome DUMESNIL nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY JEROME DUMESNIL ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL JEROME DUMESNIL BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////
#ifndef __PARSERSTATE_H__
#define __PARSERSTATE_H__
#include "Rule.h"
#include "Grammar.h"
#include "Options.h"

#include <list>
#include <ostream>

class ParserState;

struct Item
{
    enum class Type
    {
        SHIFT,
        REDUCE
    };

    const Rule &        rule;
    SymbolIterator      dot;
    const ParserState * nextState;

    bool operator ==(const Item & item) const;
    Item::Type getType(void) const;
};

class ParserState
{
    public :
        typedef std::list<Item> ItemList;

    public :
        void addRule(const Rule & rule, const SymbolIterator dot = 0);
        bool contains(const Item & item) const;
        void close(const Grammar & grammar);

        void generateActions       (std::ostream & os, Options & options, const Grammar & grammar) const;
        void generateBranchesSwitch(std::ostream & os, Options & options, const Grammar & grammar) const;
        void generateBranchesTable (std::ostream & os, Options & options, const Grammar & grammar) const;

        void printDebugActions (std::ostream & os, const Grammar & grammar) const;
        void printDebugBranches(std::ostream & os, const Grammar & grammar) const;

        bool operator ==(const ParserState & set) const;

    protected :
        void generateActionItems(std::ostream & os, Options & options, const Grammar & grammar) const;
        void generatePopFunction(std::ostream & os, Options & options, int nbStates) const;

    public :
        ItemList items;
        int      numState;

    protected :
        bool m_isClosed = false;
};

std::ostream & operator <<(std::ostream & os, const Item & item);
std::ostream & operator <<(std::ostream & os, const ParserState & itemSet);

#endif /* __PARSERSTATE_H__ */
