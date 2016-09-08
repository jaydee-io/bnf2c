////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "ParserState.h"
#include "Grammar.h"
#include "printer/PrettyPrinters.h"

#include <sstream>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
bool ParserState::contains(const Item & item) const
{
    return std::find(items.begin(), items.end(), item) != items.end();
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::assignSuccessors(const std::string & nextSymbol, const ParserState & nextState)
{
    for(auto & item : items)
        if(item.isNextSymbolEqualTo(nextSymbol))
            item.nextState = &nextState;
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::check(Errors<GeneratingError> & errors) const
{
    int                 nbReduceRules = 0;
    std::stringstream   reduceRulesError;
    int                 nbAcceptRules = 0;
    std::stringstream   acceptRulesError;

    reduceRulesError << "bnf2c can't handle parser state with multiple reduce actions" << std::endl;
    acceptRulesError << "bnf2c can't handle parser state with multiple accept actions" << std::endl;

    for(const Item & item : items)
    {
        // Check for a reduce or an accept rule
        if(item.isReduce())
        {
            if(item.rule.numRule > 1)
            {
                nbReduceRules++;
                reduceRulesError << item << std::endl;
            }
            else
            {
                nbAcceptRules++;
                acceptRulesError << item << std::endl;
            }
        }
    }

    // Check for errors
    if(nbReduceRules > 1)
        errors.list.push_back(GeneratingError({reduceRulesError.str()}));
    if(nbAcceptRules > 1)
        errors.list.push_back(GeneratingError({acceptRulesError.str()}));
}

////////////////////////////////////////////////////////////////////////////////
ParsingAction ParserState::getAction(const std::string & terminal, const std::string & endOfInputToken) const
{
    ParsingAction action = { ParsingAction::Type::ERROR, nullptr };

    for(const auto & item : items)
    {
        if(item.isShift())
        {
            // Shift rule
            if(item.dottedSymbol->name == terminal)
            {
                action.type = ParsingAction::Type::SHIFT;
                action.shiftNextState = item.nextState;
                break;
            }
        }
        if(item.isReduce())
        {
            // Reduce rule
            if(item.rule.numRule > 1 && item.isTerminalInLookaheads(terminal))
            {
                action.type = ParsingAction::Type::REDUCE;
                action.reduceRule = &item.rule;
            }
            // Accept rule
            else if(terminal == endOfInputToken && item.rule.numRule == 1)
            {
                action.type = ParsingAction::Type::ACCEPT;
                action.reduceRule = nullptr;
                break;
            }
        }
    }

    return action;
}

////////////////////////////////////////////////////////////////////////////////
const ParserState * ParserState::getGoto(const std::string & intermediate) const
{
    for(const auto & item : items)
        if(item.isShift() && item.dottedSymbol->isIntermediate() && item.dottedSymbol->name == intermediate)
            return item.nextState;

    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
bool ParserState::isSameActionForAllTerminals(const Grammar & grammar, const std::string & endOfInputToken) const
{
    const auto firstAction = getAction(endOfInputToken, endOfInputToken);

    for(const auto & terminal :grammar.terminals)
        if(getAction(terminal, endOfInputToken) != firstAction)
            return false;

    return true;
}

