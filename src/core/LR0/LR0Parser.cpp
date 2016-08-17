////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "LR0Parser.h"

#include <utility>

////////////////////////////////////////////////////////////////////////////////
Parser::StatePtr LR0Parser::createStartState(void)
{
    auto startState = std::make_unique<LR0State>();
    startState->addItem(m_grammar.getStartRule());
    return std::move(startState);
}

////////////////////////////////////////////////////////////////////////////////
std::unordered_map<std::string, Parser::StatePtr> LR0Parser::createSuccessorStates(const Parser::StatePtr & state)
{
    std::unordered_map<std::string, Parser::StatePtr> allSuccessors;

    // Create a new state for each successing symbol of the current state
    for(const auto & item : state->items)
    {
        if(item.dot < item.rule.symbols.size())
        {
            auto & newState = getStateAs<LR0State>(allSuccessors, item.getDottedSymbol().name);
            newState.addItem(item.rule, item.dot + 1);
        }
    }

    return allSuccessors;
}
