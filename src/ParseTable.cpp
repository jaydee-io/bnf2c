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
#include "ParseTable.h"

#include <unordered_map>
#include <string>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
ParseTable::ParseTable(const Grammar & grammar, Options & options)
: m_grammar(grammar), m_options(options)
{
}

////////////////////////////////////////////////////////////////////////////////
void ParseTable::generateStates(void)
{
    ParserState startSet;
    startSet.numState = 0;
    startSet.addRule(m_grammar.getStartRule());
    startSet.close(m_grammar);
    m_statesSet.push_back(startSet);

    for(StatesSet::iterator it = m_statesSet.begin(); it != m_statesSet.end(); ++it)
    {
        ParserState & state = *it;
        std::unordered_map<std::string, ParserState> newStates;

        for(Item & item : state.items)
        {
            if(item.dot < item.rule.symbols.size())
            {
                ParserState & newState = newStates[item.rule.symbols[item.dot].name];
                newState.addRule(item.rule, item.dot + 1);
                item.nextState = &newState;
            }
        }

        for(std::pair<const std::string, ParserState> & newStatePair : newStates)
        {
            ParserState & newState = newStatePair.second;

            newState.numState = m_statesSet.size();
            newState.close(m_grammar);

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
void ParseTable::generateBranchesCode(std::ostream & os) const
{
    // Branches
    if(m_options.useTableForBranches)
        generateBranchTable(os);
    else
        generateBranchSwitch(os);
}

////////////////////////////////////////////////////////////////////////////////
void ParseTable::generateParseCode(std::ostream & os) const
{
    // Function begin
    if(m_options.throwedExceptions.empty())
        os << m_options.indent << m_options.stateType << " " << m_options.parseFunctionName << "(" << m_options.tokenType << ' ' << m_options.tokenName << ')' << std::endl;
    else
        os << m_options.indent << m_options.stateType << " " << m_options.parseFunctionName << "(" << m_options.tokenType << ' ' << m_options.tokenName << ") throw(" << m_options.throwedExceptions << ")" << std::endl;
    os << m_options.indent << '{' << std::endl;
    m_options.indent++;

    // Returned value
    os << m_options.indent << m_options.valueType << ' ' << Options::VAR_RETURN << ';' << std::endl;

    // Switch on state
    os << m_options.indent << "switch(" << m_options.topState << ")" << std::endl;
    os << m_options.indent << '{' << std::endl;
    m_options.indent++;
    for(const ParserState & itemSet : m_statesSet)
        itemSet.generateActions(os, m_options, m_grammar);
    if(m_options.defaultSwitchStatement)
        os << m_options.indent << "default : return " << m_options.errorState << "; break;" << std::endl;
    m_options.indent--;
    os << m_options.indent << '}' << std::endl;

    // Return error
    os << m_options.indent << "return " << m_options.errorState << ';' << std::endl;

    // Function end
    m_options.indent--;
    os << m_options.indent << '}' << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void ParseTable::printDebug(std::ostream & output) const
{
    // Print rules
    output << "Rules :" << std::endl;
    for(Grammar::RuleMap::const_iterator it = m_grammar.rules.begin(); it != m_grammar.rules.end(); ++it)
    {
        output << "[" << it->second.numRule << "] " << it->second;
        if(!it->second.action.empty())
            output << std::endl << '"' << it->second.action << '"' << std::endl;
    }
    output << std::endl << std::endl;

    // Print parse table
    std::stringstream headerStream;
    int sizeTerminals       = 0;
    int sizeIntermediate    = 0;
    std::size_t maxSizeIntermediate = std::to_string(m_statesSet.size()).length();

    for(const std::string & terminal : m_grammar.terminals)
    {
        headerStream << terminal << '|';
        sizeTerminals += terminal.length() + 1;
    }
    headerStream << m_options.endOfInputToken << '|';
    sizeTerminals += m_options.endOfInputToken.length() + 1;

    for(const std::string & intermediate : m_grammar.intermediates)
    {
        if(intermediate != m_grammar.START_RULE)
        {
            headerStream << std::setw(std::max(intermediate.length(), maxSizeIntermediate)) << std::left << intermediate << '|';
            sizeIntermediate += std::max(intermediate.length(), maxSizeIntermediate) + 1;
        }
    }

    output << "Parse table :" << std::endl;
    output << "     |" << std::setw((sizeTerminals    - 7) / 2) << ' ' << "Actions" << std::setw((sizeTerminals    - 7) / 2) << ' ' << '|';
    output <<             std::setw((sizeIntermediate - 7) / 2) << ' ' << "Branchs" << std::setw((sizeIntermediate - 7) / 2) << ' ' << '|' << std::endl;
    output << "State|" << headerStream.str() << std::endl;
    output << std::setfill('-') << std::setw(sizeTerminals + sizeIntermediate + 6) << '-' << std::setfill(' ') << std::endl;

    for(const ParserState & itemSet : m_statesSet)
    {
        output << std::left << std::setw(5) << itemSet.numState << '|';
        itemSet.printDebugActions(output, m_grammar, m_options);
        itemSet.printDebugBranches(output, m_grammar, maxSizeIntermediate);

        output << std::endl;
    }
    output << std::endl;

    // Print items sets
    for(const ParserState & itemSet : m_statesSet)
        output << itemSet << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void ParseTable::generateBranchSwitch(std::ostream & output) const
{
    output << m_options.indent << m_options.stateType << " " << m_options.branchFunctionName << "(" << m_options.intermediateType << " intermediate)" << std::endl;
    output << m_options.indent << '{' << std::endl;
    m_options.indent++;
    output << m_options.indent << "switch(" << m_options.topState << ")" << std::endl;
    output << m_options.indent << '{' << std::endl;
    m_options.indent++;
    for(const ParserState & itemSet : m_statesSet)
        itemSet.generateBranchesSwitch(output, m_options, m_grammar);
    if(m_options.defaultSwitchStatement)
        output << m_options.indent << "default : return " << m_options.errorState << "; break;" << std::endl;
    m_options.indent--;
    output << m_options.indent << '}' << std::endl;
    output << m_options.indent << "return " << m_options.errorState << ';' << std::endl;
    m_options.indent--;
    output << m_options.indent << '}' << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void ParseTable::generateBranchTable(std::ostream & output) const
{
    output << m_options.indent << "const " << m_options.stateType << " " << m_options.branchFunctionName << "[] = {" << std::endl;
    m_options.indent++;
    for(StatesSet::const_iterator it = m_statesSet.begin(); it != m_statesSet.end(); ++it)
    {
        if(it != m_statesSet.begin())
            output << ", " << std::endl;

        it->generateBranchesTable(output, m_options, m_grammar);
    }
    output << std::endl;
    m_options.indent--;
    output << m_options.indent << "};" << std::endl;
}
