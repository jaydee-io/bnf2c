////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "LR0Parser.h"

#include <utility>

////////////////////////////////////////////////////////////////////////////////
ParserState::Ptr LR0Parser::createStartState(void)
{
    auto startState = std::make_unique<LR0State>();
    auto & startRule = m_grammar.getStartRule();
    startState->addItem(startRule, startRule.symbols.begin());
    return std::move(startState);
}

////////////////////////////////////////////////////////////////////////////////
std::unordered_map<std::string, ParserState::Ptr> LR0Parser::createSuccessorStates(const ParserState::Ptr & state)
{
    std::unordered_map<std::string, ParserState::Ptr> allSuccessors;

    // Create a new state for each successing symbol of the current state
    for(const auto & item : state->items)
    {
        if(!item.isDotAtEnd())
        {
            auto & newState = fetchOrInsertState<LR0State>(allSuccessors, item.dottedSymbol->name);
            newState.addItem(item.rule, item.dottedSymbol + 1);
        }
    }

    return allSuccessors;
}
