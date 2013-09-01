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
void ParseTable::checkGrammar(void) const throw(GeneratingError)
{
    Dictionnary::Index startIndex = m_grammar.intermediates[Grammar::START_RULE];

    if(startIndex == m_grammar.intermediates.end())
        throw GeneratingError({ "No rule named \"" + Grammar::START_RULE + "\" found !!!" });

    Grammar::RuleMap::size_type nbStartsRules = m_grammar.rules.count(startIndex);

    if(nbStartsRules == 0)
            throw GeneratingError({ "No rule named \"" + Grammar::START_RULE + "\" found !!!" });
    else if(nbStartsRules > 1)
            throw GeneratingError({ "Multiple start rules \"" + Grammar::START_RULE + "\" found !!!" });
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
    if(m_options.throwedExceptions.empty())
        os << m_options.indent << m_options.stateType << " " << m_options.parseFunctionName << "(" << m_options.tokenType << " token)" << std::endl;
    else
        os << m_options.indent << m_options.stateType << " " << m_options.parseFunctionName << "(" << m_options.tokenType << " token) throw(" << m_options.throwedExceptions << ")" << std::endl;
    os << m_options.indent << '{' << std::endl;
    m_options.indent++;
    os << m_options.indent << "switch(" << m_options.topState << ")" << std::endl;
    os << m_options.indent << '{' << std::endl;
    m_options.indent++;
    for(const ParserState & itemSet : m_statesSet)
        itemSet.generateActions(os, m_options, m_grammar);
    if(m_options.defaultSwitchStatement)
        os << m_options.indent << "default : return " << m_options.errorState << "; break;" << std::endl;
    m_options.indent--;
    os << m_options.indent << '}' << std::endl;
    os << m_options.indent << "return " << m_options.errorState << ';' << std::endl;
    m_options.indent--;
    os << m_options.indent << '}' << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void ParseTable::printDebug(std::ostream & output) const
{
    // Print rules
    output << "Rules :" << std::endl << std::endl;
    for(Grammar::RuleMap::const_iterator it = m_grammar.rules.begin(); it != m_grammar.rules.end(); ++it)
        output << "[" << it->second.numRule << "] " << it->second << std::endl;
    output << std::endl << std::endl;

    // Print parse table
    std::stringstream headerStream;
    int sizeTerminals    = 0;
    int sizeIntermediate = 0;

    for(const std::string & terminal : m_grammar.terminals)
    {
        headerStream << std::setw(m_grammar.terminals.getMaxSrtingLength()) << std::left << terminal << '|';
        sizeTerminals += m_grammar.terminals.getMaxSrtingLength() + 1;
    }
    for(const std::string & intermediate : m_grammar.intermediates)
    {
        if(intermediate != m_grammar.START_RULE)
        {
            headerStream << std::setw(m_grammar.intermediates.getMaxSrtingLength()) << std::left << intermediate << '|';
            sizeIntermediate += m_grammar.intermediates.getMaxSrtingLength() + 1;
        }
    }

    output << "Parse states :" << std::endl << std::endl;
    output << "     |" << std::setw((sizeTerminals    - 7) / 2) << ' ' << "Actions" << std::setw((sizeTerminals    - 7) / 2) << ' ' << '|';
    output <<             std::setw((sizeIntermediate - 7) / 2) << ' ' << "Branchs" << std::setw((sizeIntermediate - 7) / 2) << ' ' << '|' << std::endl;
    output << "State|" << headerStream.str() << std::endl;
    output << std::setfill('-') << std::setw(sizeTerminals + sizeIntermediate + 6) << '-' << std::setfill(' ') << std::endl;

    for(const ParserState & itemSet : m_statesSet)
    {
        output << std::left << std::setw(5) << itemSet.numState << '|';
        itemSet.printDebugActions(output, m_grammar);
        itemSet.printDebugBranches(output, m_grammar);

        output << std::endl;
    }
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
