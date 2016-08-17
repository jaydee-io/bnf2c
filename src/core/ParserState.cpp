////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "ParserState.h"
#include "Grammar.h"

#include <sstream>
#include <algorithm>
#include <iomanip>

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
void ParserState::printDebugActions(std::ostream & os, const Grammar & grammar, const Options & options) const
{
    auto & firstItem = items.front();
    for(const std::string & terminal : grammar.terminals)
    {
        ItemList::const_iterator it;

        // Shift rule
        for(it = items.begin(); it != items.end(); ++it)
        {
            if(it->isShift() && (it->rule.symbols[it->dot].name == terminal))
            {
                os << 'S' << std::setw(terminal.length() - 1) << std::left << (it->nextState != nullptr ? it->nextState->numState : -1) << '|';
                break;
            }
        }

        // Reduce
        if(it == items.end())
        {
            if(firstItem.isReduce() && firstItem.rule.numRule > 1)
                os << 'R' << std::setw(terminal.length() - 1) << std::left << firstItem.rule.numRule << '|';
            else
                os << std::setw(terminal.length() + 1) << std::right << '|';
        }
    }

    // End of input special token
    if(firstItem.isReduce() && firstItem.rule.numRule == 1)
    {
        // Accept rule
        os << std::setw(options.endOfInputToken.length()) << std::left << "ACC" << '|';
    }
    else
    {
        // Reduce
        if(firstItem.isReduce() && firstItem.rule.numRule > 1)
            os << 'R' << std::setw(options.endOfInputToken.length() - 1) << std::left << firstItem.rule.numRule << '|';
        else
            os << std::setw(options.endOfInputToken.length() + 1) << std::right << '|';
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::printDebugBranches(std::ostream & os, const Grammar & grammar, std::size_t size) const
{
    for(const std::string & intermediate : grammar.intermediates)
    {
        if(intermediate != grammar.START_RULE)
        {
            ItemList::const_iterator it;
            for(it = items.begin(); it != items.end(); ++it)
            {
                const Symbol & symbol = it->rule.symbols[it->dot];

                if(it->isShift() && symbol.isIntermediate() && (symbol.name == intermediate))
                {
                    os << std::setw(std::max(intermediate.length(), size)) << std::left << (it->nextState != nullptr ? it->nextState->numState : -1) << '|';
                    break;
                }
            }

            if(it == items.end())
                os << std::setw(std::max(intermediate.length(), size) + 1) << std::right << '|';
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
bool ParserState::operator ==(const ParserState & set) const
{
    if(&set == this)
        return true;

    return items == set.items;
}

////////////////////////////////////////////////////////////////////////////////
bool ParserState::symbolNeedsToBeClosed(const Symbol & symbol)
{
    if(symbol.isIntermediate() && symbolsAlreadyClosed.find(symbol.name) == symbolsAlreadyClosed.end())
    {
        symbolsAlreadyClosed.insert(symbol.name);
        return true;
    }

    return false;
}


////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const ParserState & itemSet)
{
    os << "Set " << itemSet.numState << std::endl;

    for(Item item : itemSet.items)
        os << item << std::endl;

    return os;
}
