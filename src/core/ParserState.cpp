////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "ParserState.h"

#include <map>
#include <unordered_set>
#include <sstream>
#include <algorithm>
#include <iomanip>

////////////////////////////////////////////////////////////////////////////////
bool Item::operator ==(const Item & item) const
{
    if(&item == this)
        return true;

    return (dot == item.dot) && (rule == item.rule);
}

////////////////////////////////////////////////////////////////////////////////
bool Item::operator <(const Item & item) const
{
    if(&item == this)
        return false;

    if(rule.numRule != item.rule.numRule)
        return (rule.numRule < item.rule.numRule);
    else
        return (dot < item.dot);
}

////////////////////////////////////////////////////////////////////////////////
Item::ActionType Item::getType(void) const
{
    if(dot >= rule.symbols.size())
        return ActionType::REDUCE;
    else
        return ActionType::SHIFT;
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::addRule(const Rule & rule, const SymbolIterator dot)
{
    items.push_back(Item({rule, dot, nullptr}));
}

////////////////////////////////////////////////////////////////////////////////
bool ParserState::contains(const Item & item) const
{
    return std::find(items.begin(), items.end(), item) != items.end();
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::close(const Grammar & grammar)
{
    if(!m_isClosed)
    {
        std::unordered_set<std::string>  symbolsAlreadyClosed;

        for(ItemList::iterator it = items.begin(); it != items.end(); ++it)
        {
            // Check for a reduce or an accept rule
            if(it->getType() == Item::ActionType::REDUCE)
            {
                if(it->rule.numRule > 1)
                    m_reduceRule = &(*it);
                else
                    m_isAnAcceptRule = true;
            }

            // Close the item set
            if(it->dot < it->rule.symbols.size())
            {
                const Symbol & symbol = it->rule.symbols[it->dot];

                if(symbol.type == Symbol::Type::INTERMEDIATE)
                {
                    if(symbolsAlreadyClosed.find(symbol.name) == symbolsAlreadyClosed.end())
                    {
                        symbolsAlreadyClosed.insert(symbol.name);

                        Grammar::RuleRange range = grammar[symbol.name];
                        for(Grammar::RuleIterator ruleIt = range.first; ruleIt != range.second; ++ruleIt)
                            addRule(ruleIt->second);
                    }
                }
            }
        }

        m_isClosed = true;
    }
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
        if(item.getType() == Item::ActionType::REDUCE)
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
bool ParserState::isAnAcceptRule(void) const
{
    return m_isAnAcceptRule;
}

////////////////////////////////////////////////////////////////////////////////
Item * ParserState::getReduceRule(void) const
{
    return m_reduceRule;
}


////////////////////////////////////////////////////////////////////////////////
void ParserState::printDebugActions(std::ostream & os, const Grammar & grammar, const Options & options) const
{
    for(const std::string & terminal : grammar.terminals)
    {
        ItemList::const_iterator it;

        // Shift rule
        for(it = items.begin(); it != items.end(); ++it)
        {
            if((it->getType() == Item::ActionType::SHIFT) && (it->rule.symbols[it->dot].name == terminal))
            {
                os << 'S' << std::setw(terminal.length() - 1) << std::left << (it->nextState != nullptr ? it->nextState->numState : -1) << '|';
                break;
            }
        }

        // Reduce
        if(it == items.end())
        {
            if(m_reduceRule)
                os << 'R' << std::setw(terminal.length() - 1) << std::left << m_reduceRule->rule.numRule << '|';
            else
                os << std::setw(terminal.length() + 1) << std::right << '|';
        }
    }

    // End of input special token
    if(m_isAnAcceptRule)
    {
        // Accept rule
        os << std::setw(options.endOfInputToken.length()) << std::left << "ACC" << '|';
    }
    else
    {
        // Reduce
        if(m_reduceRule)
            os << 'R' << std::setw(options.endOfInputToken.length() - 1) << std::left << m_reduceRule->rule.numRule << '|';
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

                if((it->getType() == Item::ActionType::SHIFT) && (symbol.type == Symbol::Type::INTERMEDIATE) && (symbol.name == intermediate))
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
std::ostream & operator <<(std::ostream & os, const Item & item)
{
    if(item.getType() == Item::ActionType::REDUCE)
        os << "[R" << item.rule.numRule << "] ";
    else if(item.getType() == Item::ActionType::SHIFT)
        os << "[S" << (item.nextState != nullptr ? item.nextState->numState : -1) << "] ";

    os << "<" << item.rule.name << "> ::=";

    for(SymbolIterator it = 0; it < item.rule.symbols.size(); ++it)
    {
        if(it == item.dot)
            os << " •" << item.rule.symbols[it];
        else
            os << " " << item.rule.symbols[it];
    }

    if(item.dot == item.rule.symbols.size())
        os << " •";

    return os;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const ParserState & itemSet)
{
    os << "Set " << itemSet.numState << std::endl;

    for(Item item : itemSet.items)
        os << item << std::endl;

    return os;
}
