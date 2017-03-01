////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "Parser.h"
#include "Grammar.h"
#include "config/Options.h"
#include "printer/PrettyPrinters.h"

#include <string>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <sstream>

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
ParserState::Ptr & Parser::addNewState(ParserState::Ptr && state)
{
    state->numState = m_states.size();
    state->close(m_grammar);

    return addOrMergeState(std::forward<ParserState::Ptr>(state));
}

////////////////////////////////////////////////////////////////////////////////
ParserState::Ptr & Parser::addOrMergeState(ParserState::Ptr && newState)
{
    auto mergeableSate = std::find_if(m_states.begin(), m_states.end(), [&newState](const auto & state) -> bool { return state->isMergeableWith(newState); });
    if(mergeableSate == m_states.end())
        return *m_states.insert(mergeableSate, std::forward<ParserState::Ptr>(newState));
    else
    {
        (*mergeableSate)->merge(newState);

        // Recusively propagate lookahead of first item to all successors
        mergeSucessorsLookahead(*(*mergeableSate), newState->items.begin()->lookaheads);

        return *mergeableSate;
    }
}

////////////////////////////////////////////////////////////////////////////////
void Parser::mergeSucessorsLookahead(ParserState & state, SymbolSet & lookaheads)
{
    auto firstItemIt = state.items.begin();

    if(firstItemIt != state.items.end())
    {
        firstItemIt->lookaheads.insert(lookaheads.begin(), lookaheads.end());

        if(firstItemIt->isShift() && firstItemIt->nextState)
            mergeSucessorsLookahead(*(firstItemIt->nextState), lookaheads);
    }
}

////////////////////////////////////////////////////////////////////////////////
void Parser::check(void)
{
    for(const auto & state : m_states)
        state->check(errors);
}

////////////////////////////////////////////////////////////////////////////////
const Parser::States & Parser::getStates(void) const
{
    return m_states;
}

////////////////////////////////////////////////////////////////////////////////
const Grammar & Parser::getGrammar(void) const
{
    return m_grammar;
}

////////////////////////////////////////////////////////////////////////////////
const Options & Parser::getOptions(void) const
{
    return m_options;
}

