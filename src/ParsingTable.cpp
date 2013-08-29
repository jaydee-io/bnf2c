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
void ParsingTable::generateStates(const Grammar & grammar)
{
    ParserState startSet;
    startSet.numState = 0;
    startSet.addRule(grammar.getStartRule());
    startSet.close(grammar);
    m_statesSet.push_back(startSet);

    for(StatesSet::iterator it = m_statesSet.begin(); it != m_statesSet.end(); ++it)
    {
        ParserState & state = *it;
        std::unordered_map<Dictionnary::Index, ParserState> newStates;

        for(Item & item : state.items)
        {
            if(item.dot < item.rule.symbols.size())
            {
                ParserState & newState = newStates[item.rule.symbols[item.dot].name];
                newState.addRule(item.rule, item.dot + 1);
                item.nextState = &newState;
            }
        }

        for(std::pair<const Dictionnary::Index, ParserState> & newStatePair : newStates)
        {
            ParserState & newState = newStatePair.second;

            newState.numState = m_statesSet.size();
            newState.close(grammar);

            StatesSet::iterator it = std::find(m_statesSet.begin(), m_statesSet.end(), newState);
            if(it == m_statesSet.end())
            {
                m_statesSet.push_back(std::move(newState));

                for(Item & item : state.items)
                    if(item.nextState == &newState)
                        item.nextState = &m_statesSet.back();
            }
            else
            {
                for(Item & item : state.items)
                    if(item.nextState == &newState)
                        item.nextState = &(*it);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParsingTable::outputTable(std::ostream & output, Options & options, const Grammar & grammar) const
{
    // Branches
    if(options.useTableForBranches)
        outputBranchTable(output, options, grammar);
    else
        outputBranchSwitch(output, options, grammar);

    output << std::endl;

    // Actions
    outputActions(output, options, grammar);
}

////////////////////////////////////////////////////////////////////////////////
void ParsingTable::outputDebug(std::ostream & output) const
{
    for(const ParserState & itemSet : m_statesSet)
    {
        output << "--------------------------------------------------" << std::endl;
        output << itemSet << std::endl;

        for(const Item & item : itemSet.items)
        {
            switch(item.getType())
            {
                case Item::Type::REDUCE :
                    output << "[R] using rule \"" << item.rule << "\"" << std::endl;
                    break;
                case Item::Type::SHIFT :
                    switch(item.rule.symbols[item.dot].type)
                    {
                        case Symbol::Type::TERMINAL     : output << item.rule.symbols[item.dot].name << " => [S " << (item.nextState != nullptr ? item.nextState->numState : -1) << "]" << std::endl; break;
                        case Symbol::Type::INTERMEDIATE : output << item.rule.symbols[item.dot].name << " => [B " << (item.nextState != nullptr ? item.nextState->numState : -1) << "]" << std::endl; break;
                        default                         : output << "Unknown what to do for \"" << item << "\"" << std::endl;        break;
                    }

                    break;
                default :
                    output << "Unknown what to do for \"" << item << "\"" << std::endl;
                    break;
            }
        }

        output << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParsingTable::outputActions(std::ostream & output, Options & options, const Grammar & grammar) const
{
    if(options.throwedExceptions.empty())
        output << options.indent << options.stateType << " " << options.parseFunctionName << "(" << options.tokenType << " token)" << std::endl;
    else
        output << options.indent << options.stateType << " " << options.parseFunctionName << "(" << options.tokenType << " token) throw(" << options.throwedExceptions << ")" << std::endl;
    output << options.indent << '{' << std::endl;
    options.indent++;
    output << options.indent << "switch(" << options.topState << ")" << std::endl;
    output << options.indent << '{' << std::endl;
    options.indent++;
    for(const ParserState & itemSet : m_statesSet)
        itemSet.outputActions(output, options, grammar);
    if(options.defaultSwitchStatement)
        output << options.indent << "default : return " << options.errorState << "; break;" << std::endl;
    options.indent--;
    output << options.indent << '}' << std::endl;
    output << options.indent << "return " << options.errorState << ';' << std::endl;
    options.indent--;
    output << options.indent << '}' << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void ParsingTable::outputBranchSwitch(std::ostream & output, Options & options, const Grammar & grammar) const
{
    output << options.indent << options.stateType << " " << options.branchFunctionName << "(" << options.intermediateType << " intermediate)" << std::endl;
    output << options.indent << '{' << std::endl;
    options.indent++;
    output << options.indent << "switch(" << options.topState << ")" << std::endl;
    output << options.indent << '{' << std::endl;
    options.indent++;
    for(const ParserState & itemSet : m_statesSet)
        itemSet.outputBranchesSwitch(output, options, grammar);
    if(options.defaultSwitchStatement)
        output << options.indent << "default : return " << options.errorState << "; break;" << std::endl;
    options.indent--;
    output << options.indent << '}' << std::endl;
    output << options.indent << "return " << options.errorState << ';' << std::endl;
    options.indent--;
    output << options.indent << '}' << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void ParsingTable::outputBranchTable(std::ostream & output, Options & options, const Grammar & grammar) const
{
    output << options.indent << "const " << options.stateType << " " << options.branchFunctionName << "[] = {" << std::endl;
    options.indent++;
    for(StatesSet::const_iterator it = m_statesSet.begin(); it != m_statesSet.end(); ++it)
    {
        if(it != m_statesSet.begin())
            output << ", " << std::endl;

        it->outputBranchesTable(output, options, grammar);
    }
    output << std::endl;
    options.indent--;
    output << options.indent << "};" << std::endl;
}
