////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "ParseTable.h"

#include <unordered_map>
#include <string>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <sstream>
#include <cstring>

#define CENTER(msg, maxSize)    std::setw((maxSize - ::strlen(msg)) / 2) << ' ' << msg << std::setw(maxSize - ((maxSize - ::strlen(msg)) / 2) - ::strlen(msg) - 1) << ' '

////////////////////////////////////////////////////////////////////////////////
ParseTable::ParseTable(const Grammar & grammar, Options & options)
: m_grammar(grammar), m_options(options)
{
}

////////////////////////////////////////////////////////////////////////////////
void ParseTable::generateStates(void)
{
    ParserState startState;
    startState.addRule(m_grammar.getStartRule());

    addNewState(std::forward<ParserState>(startState));

    for(auto & state : m_states)
    {
        auto allSuccessors = createSuccessorStates(state);

        for(auto & successorPair : allSuccessors)
        {
            auto & successor = addNewState(std::move(successorPair.second));
            state.assignSuccessors(successorPair.first, successor);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
ParserState & ParseTable::addNewState(ParserState && state)
{
    state.numState = m_states.size();
    state.close(m_grammar);

    return addOrMergeState(std::forward<ParserState>(state));
}

////////////////////////////////////////////////////////////////////////////////
std::unordered_map<std::string, ParserState> ParseTable::createSuccessorStates(const ParserState & state)
{
    std::unordered_map<std::string, ParserState> allSuccessors;

    // Create a new state for each successing symbol of the current state
    for(const auto & item : state.items)
        if(item.dot < item.rule.symbols.size())
            allSuccessors[item.getDottedSymbol().name].addRule(item.rule, item.dot + 1);

    return allSuccessors;
}

////////////////////////////////////////////////////////////////////////////////
ParserState & ParseTable::addOrMergeState(ParserState && state)
{
    auto itState = std::find(m_states.begin(), m_states.end(), state);
    if(itState == m_states.end())
        return *m_states.insert(itState, std::forward<ParserState>(state));
    else
        return *itState;
}

////////////////////////////////////////////////////////////////////////////////
void ParseTable::check(void)
{
    for(const ParserState & state : m_states)
        state.check(errors);
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
        output << std::endl;
    }
    output << std::endl << std::endl;

    // Print parse table
    std::stringstream headerStream;
    int sizeTerminals       = 0;
    int sizeIntermediate    = 0;
    std::size_t maxSizeIntermediate = std::to_string(m_states.size()).length();

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
    output << "     |" << CENTER("Actions", sizeTerminals) << '|';
    output <<             CENTER("Branchs", sizeIntermediate) << '|' << std::endl;
    output << "State|" << headerStream.str() << std::endl;
    output << std::setfill('-') << std::setw(sizeTerminals + sizeIntermediate + 6) << '-' << std::setfill(' ') << std::endl;

    for(const ParserState & itemSet : m_states)
    {
        output << std::left << std::setw(5) << itemSet.numState << '|';
        itemSet.printDebugActions(output, m_grammar, m_options);
        itemSet.printDebugBranches(output, m_grammar, maxSizeIntermediate);

        output << std::endl;
    }
    output << std::endl;

    // Print items sets
    for(const ParserState & itemSet : m_states)
        output << itemSet << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
const ParseTable::States & ParseTable::getStates(void) const
{
    return m_states;
}
