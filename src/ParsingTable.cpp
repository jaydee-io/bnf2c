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
#include "ParsingTable.h"

#include <unordered_map>
#include <string>
#include <algorithm>
#include <iterator>

////////////////////////////////////////////////////////////////////////////////
void ParsingTable::generate(const Grammar & grammar)
{
    generateItemSets(m_statesSet, grammar);
    generateNewStates(m_statesSet);

//    outputItemSets(std::cout, m_statesSet);
}

////////////////////////////////////////////////////////////////////////////////
void ParsingTable::output(OutputFormatter & of, const Grammar & grammar) const
{
    // Branches
    if(of.useTableForBranches)
        outputBranchTable(of, grammar);
    else
        outputBranchSwitch(of, grammar);

    of.outputStream << std::endl;

    // Actions
    outputActions(of, grammar);
}

////////////////////////////////////////////////////////////////////////////////
void ParsingTable::generateItemSets(StatesSet & itemSets, const Grammar & grammar)
{
    ParserState startSet;
    startSet.numState = 0;
    startSet.addRule(grammar.getStartRule());
    startSet.close(grammar);
    itemSets.push_back(startSet);

    for(StatesSet::iterator it = itemSets.begin(); it != itemSets.end(); ++it)
    {
        ParserState & state = *it;
        std::unordered_map<Dictionnary::Index, ParserState> newStates;

        for(Item & item : state.items)
        {
            if(item.dot < item.rule.symbols.size())
                newStates[item.rule.symbols[item.dot].name].addRule(item.rule, item.dot + 1);
        }

        for(std::pair<const Dictionnary::Index, ParserState> & newState : newStates)
        {
            newState.second.numState = itemSets.size();
            newState.second.close(grammar);

            if(std::find(itemSets.begin(), itemSets.end(), newState.second) == itemSets.end())
                itemSets.push_back(newState.second);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParsingTable::generateNewStates(StatesSet & itemSets)
{
    for(ParserState & set : itemSets)
    {
        for(Item & item : set.items)
            if(item.getType() == Item::Type::SHIFT)
                item.nextState = findSetContaining(itemSets, item);
    }
}

////////////////////////////////////////////////////////////////////////////////
const ParserState * ParsingTable::findSetContaining(const StatesSet & itemSets, const Item & item)
{
    Item itemShifted = item;
    itemShifted.dot++;

    for(const ParserState & set : itemSets)
    {
        if(set.contains(itemShifted))
            return &set;
    }

    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
void ParsingTable::outputItemSets(std::ostream & os, const StatesSet & itemSets) const
{
    for(const ParserState & itemSet : itemSets)
    {
        os << "--------------------------------------------------" << std::endl;
        os << itemSet << std::endl;

        for(const Item & item : itemSet.items)
        {
            switch(item.getType())
            {
                case Item::Type::REDUCE :
                    std::cout << "[R] using rule \"" << item.rule << "\"" << std::endl;
                    break;
                case Item::Type::SHIFT :
                    switch(item.rule.symbols[item.dot].type)
                    {
                        case Symbol::Type::TERMINAL     : std::cout << item.rule.symbols[item.dot].name << " => [S " << (item.nextState != nullptr ? item.nextState->numState : -1) << "]" << std::endl; break;
                        case Symbol::Type::INTERMEDIATE : std::cout << item.rule.symbols[item.dot].name << " => [B " << (item.nextState != nullptr ? item.nextState->numState : -1) << "]" << std::endl; break;
                        default                         : std::cout << "Unknown what to do for \"" << item << "\"" << std::endl;        break;
                    }

                    break;
                default :
                    std::cout << "Unknown what to do for \"" << item << "\"" << std::endl;
                    break;
            }
        }

        os << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParsingTable::outputActions(OutputFormatter & of, const Grammar & grammar) const
{
    of.outputStream << of.indent << of.stateType << " " << of.parseFunctionName << "(" << of.tokenType << " token)" << std::endl;
    of.outputStream << of.indent << '{' << std::endl;
    of.indent++;
    of.outputStream << of.indent << "switch(" << of.topState << ")" << std::endl;
    of.outputStream << of.indent << '{' << std::endl;
    of.indent++;
    for(const ParserState & itemSet : m_statesSet)
        itemSet.outputActions(of, grammar);
    if(of.defaultSwitchStatement)
        of.outputStream << of.indent << "default : return " << of.errorState << "; break;" << std::endl;
    of.indent--;
    of.outputStream << of.indent << '}' << std::endl;
    of.outputStream << of.indent << "return " << of.errorState << ';' << std::endl;
    of.indent--;
    of.outputStream << of.indent << '}' << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void ParsingTable::outputBranchSwitch(OutputFormatter & of, const Grammar & grammar) const
{
    of.outputStream << of.indent << of.stateType << " " << of.branchFunctionName << "(" << of.intermediateType << " intermediate)" << std::endl;
    of.outputStream << of.indent << '{' << std::endl;
    of.indent++;
    of.outputStream << of.indent << "switch(" << of.topState << ")" << std::endl;
    of.outputStream << of.indent << '{' << std::endl;
    of.indent++;
    for(const ParserState & itemSet : m_statesSet)
        itemSet.outputBranchesSwitch(of, grammar);
    if(of.defaultSwitchStatement)
        of.outputStream << of.indent << "default : return " << of.errorState << "; break;" << std::endl;
    of.indent--;
    of.outputStream << of.indent << '}' << std::endl;
    of.outputStream << of.indent << "return " << of.errorState << ';' << std::endl;
    of.indent--;
    of.outputStream << of.indent << '}' << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void ParsingTable::outputBranchTable(OutputFormatter & of, const Grammar & grammar) const
{
    of.outputStream << of.indent << "const " << of.stateType << " " << of.branchFunctionName << "[] = {" << std::endl;
    of.indent++;
    for(StatesSet::const_iterator it = m_statesSet.begin(); it != m_statesSet.end(); ++it)
    {
        if(it != m_statesSet.begin())
            of.outputStream << ", " << std::endl;

        it->outputBranchesTable(of, grammar);
    }
    of.outputStream << std::endl;
    of.indent--;
    of.outputStream << of.indent << "};" << std::endl;
}
