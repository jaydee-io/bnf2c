////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "Parser.h"

#include <unordered_map>
#include <string>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <sstream>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////
Parser::Parser(const Grammar & grammar, Options & options)
: m_grammar(grammar), m_options(options)
{
}

////////////////////////////////////////////////////////////////////////////////
void Parser::generateStates(void)
{
    addNewState(createStartState());
    for(auto & state : m_states)
    {
        auto allSuccessors = createSuccessorStates(state);

        for(auto & successorPair : allSuccessors)
        {
            auto & successor = addNewState(std::move(successorPair.second));
            state->assignSuccessors(successorPair.first, *successor);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
Parser::StatePtr & Parser::addNewState(Parser::StatePtr && state)
{
    state->numState = m_states.size();
    state->close(m_grammar);

    return addOrMergeState(std::forward<Parser::StatePtr>(state));
}

////////////////////////////////////////////////////////////////////////////////
Parser::StatePtr & Parser::addOrMergeState(Parser::StatePtr && state)
{
    auto itState = std::find_if(m_states.begin(), m_states.end(), [&state](const auto & rhs) -> bool { return *state == *rhs; });
    if(itState == m_states.end())
        return *m_states.insert(itState, std::forward<Parser::StatePtr>(state));
    else
        return *itState;
}

////////////////////////////////////////////////////////////////////////////////
void Parser::check(void)
{
    for(const auto & state : m_states)
        state->check(errors);
}

#define CENTER(msg, maxSize)    std::setw((maxSize - ::strlen(msg)) / 2) << ' ' << msg << std::setw(maxSize - ((maxSize - ::strlen(msg)) / 2) - ::strlen(msg) - 1) << ' '

////////////////////////////////////////////////////////////////////////////////
void Parser::printDebug(std::ostream & output) const
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
    output << "     |" << CENTER("Actions", std::max(sizeTerminals, 7)) << '|';
    output <<             CENTER("Branchs", std::max(sizeIntermediate, 7)) << '|' << std::endl;
    output << "State|" << headerStream.str() << std::endl;
    output << std::setfill('-') << std::setw(sizeTerminals + sizeIntermediate + 6) << '-' << std::setfill(' ') << std::endl;

    for(const auto & state : m_states)
    {
        output << std::left << std::setw(5) << state->numState << '|';
        state->printDebugActions(output, m_grammar, m_options);
        state->printDebugBranches(output, m_grammar, maxSizeIntermediate);

        output << std::endl;
    }
    output << std::endl;

    // Print items sets
    for(const auto & state : m_states)
        output << *state << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
const Parser::States & Parser::getStates(void) const
{
    return m_states;
}
