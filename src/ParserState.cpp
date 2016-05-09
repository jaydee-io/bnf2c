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
void ParserState::generateActions(std::ostream & os, Options & options, const Grammar & grammar) const
{
    os << options.indent << "case " << numState << " :" << std::endl;

    options.indent++;
    generateActionItems(os, options, grammar);
    options.indent--;
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::generateBranchesSwitch(std::ostream & os, Options & options, const Grammar & grammar) const
{
    std::unordered_set<std::string> outCases;
    options.indent++++;
    for(const Item & item : items)
    {
        std::stringstream os;

        if((item.dot < item.rule.symbols.size()) && (item.rule.symbols[item.dot].type == Symbol::Type::INTERMEDIATE))
        {
            os << options.indent << "case " << grammar.intermediates.index(item.rule.symbols[item.dot].name) << " : ";
            if(item.nextState != nullptr)
                os << "return " << item.nextState->numState << ";" << std::endl;
            else
                os << "return " << options.errorState << ";" << std::endl;
            outCases.insert(os.str());
        }
    }
    options.indent----;

    if(!outCases.empty())
    {
        // Epilog
        os << options.indent << "case " << numState << " :";
        options.indent++;

        os << std::endl;
        os << options.indent << "switch(" << options.intermediateName << ")" << std::endl;
        os << options.indent << "{" << std::endl;
        for(const std::string & str : outCases)
            os << str;

        if(options.defaultSwitchStatement)
        {
            options.indent++;
            os << options.indent << "default : return " << options.errorState << ";" << std::endl;
            options.indent--;
        }

        os << options.indent << "}" << std::endl;
        os << options.indent << "break;" << std::endl;
        options.indent--;
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::generateBranchesTable(std::ostream & os, Options & options, const Grammar & grammar) const
{
    os << options.indent;
    for(Dictionary::const_iterator itIntermediate = grammar.intermediates.begin(); itIntermediate != grammar.intermediates.end(); ++itIntermediate)
    {
        if(itIntermediate != grammar.intermediates.begin())
            os << ", ";

        ItemList::const_iterator itItem;
        for(itItem = items.begin(); itItem != items.end(); ++itItem)
            if((itItem->dot < itItem->rule.symbols.size()) && (itItem->rule.symbols[itItem->dot].name == *itIntermediate))
                break;

        if(itItem != items.end() && (itItem->nextState != nullptr))
            os << itItem->nextState->numState;
        else
            os << options.errorState;
    }
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
void ParserState::generateActionItems(std::ostream & os, Options & options, const Grammar & grammar) const
{
    // If the first action is a reduce (not the accept one) whatever the terminal, don't generate a switch
    if((items.size() >= 1) && (items.begin()->getType() == Item::ActionType::REDUCE) && !m_isAnAcceptRule)
    {
        generateReduceAction(*items.begin(), os, options, grammar);
        return;
    }

    // Regroup all cases of an item
    std::map<Item, std::set<std::string> > cases;
    for(const std::string & terminal : grammar.terminals)
    {
        ItemList::const_iterator it;

        // Shift rule
        for(it = items.begin(); it != items.end(); ++it)
        {
            if((it->getType() == Item::ActionType::SHIFT) && (it->rule.symbols[it->dot].name == terminal))
            {
                cases[*it].insert(terminal);
                break;
            }
        }

        // Reduce
        if(it == items.end() && m_reduceRule)
            cases[*m_reduceRule].insert(terminal);
    }

    // If there is a reduce rule (not the accept one), reduce also when current token is the end of input
    if(!m_isAnAcceptRule && m_reduceRule)
        cases[*m_reduceRule].insert(options.endOfInputToken);

    // Switch on terminal
    os << options.indent << "switch(" << checkedStringReplace(options.getTypeOfToken, Options::VAR_TOKEN, options.tokenName) << ")" << std::endl;
    os << options.indent << "{" << std::endl;
    options.indent++;

    for(const std::pair<const Item, std::set<std::string> > & casesOfItem : cases)
    {
        // Generate all cases
        for(std::set<std::string>::const_iterator it = casesOfItem.second.begin(); it != casesOfItem.second.end(); ++it)
        {
            os << options.indent << "case " << options.tokenPrefix << *it << " : ";

            if(casesOfItem.second.size() > 1)
                os << std::endl;
        }

        // Shift action
        if((casesOfItem.first.getType() == Item::ActionType::SHIFT) && (casesOfItem.first.rule.symbols[casesOfItem.first.dot].type == Symbol::Type::TERMINAL))
        {
            if(casesOfItem.second.size() == 1)
                generateShiftAction(casesOfItem.first, os, options, grammar);
            else
            {
                options.indent++;
                os << options.indent;
                generateShiftAction(casesOfItem.first, os, options, grammar);
                options.indent--;
            }
        }
        // Reduce action
        else if(casesOfItem.first.getType() == Item::ActionType::REDUCE)
        {
            os << std::endl;
            generateReduceAction(casesOfItem.first, os, options, grammar);
        }
    }

    // Accept rule
    if(m_isAnAcceptRule)
        os << options.indent << "case " << options.tokenPrefix << options.endOfInputToken << " : return " << options.acceptState << ";" << std::endl;

    // Default switch statement
    if(options.defaultSwitchStatement)
        os << options.indent << "default : return " << options.errorState << ";" << std::endl;

    options.indent--;
    os << options.indent << "}" << std::endl;
    os << options.indent << "break;" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::generateReduceAction(const Item & item, std::ostream & os, Options & options, const Grammar & grammar) const
{
    os << options.indent << '{' << std::endl;
    options.indent++;

    // No action specified by user, assume "$$ = $1"
    if(item.rule.action.empty())
    {
        std::string replacement = checkedStringReplace(options.getValue, Options::VAR_VALUE_IDX, std::to_string(item.rule.symbols.size() - 1));
        replacement = checkedStringReplace(replacement, Options::VAR_TYPE, grammar.getIntermediateType(item.rule.name));
        os << options.indent  << Options::VAR_RETURN << " = " << replacement << ';' << std::endl;
    }
    // Rule action code
    else
    {
        if(item.rule.action.find_first_of("\n\r") != std::string::npos)
            os << item.rule.action << std::endl << std::endl;
        else
            os << options.indent << item.rule.action << std::endl << std::endl;
    }

    // Values stack
    os << options.indent << checkedStringReplace(options.popValues, Options::VAR_NB_VALUES, std::to_string(item.rule.symbols.size()))  << std::endl;
    os << options.indent << checkedStringReplace(options.pushValue, Options::VAR_VALUE,     Options::VAR_RETURN)                       << std::endl;

    // States stack
    os << options.indent << checkedStringReplace(options.popState, Options::VAR_NB_STATES, std::to_string(item.rule.symbols.size())) << std::endl;

    // New state
    if(options.useTableForBranches)
        os << options.indent << "return " << options.branchFunctionName << "[(" << grammar.intermediates.size() << "*" << options.topState << ") + " << grammar.intermediates.index(item.rule.name) << "];" << std::endl;
    else
        os << options.indent << "return " << options.branchFunctionName << "(" << grammar.intermediates.index(item.rule.name) << ");" << std::endl;

    options.indent--;
    os << options.indent << '}' << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::generateShiftAction(const Item & item, std::ostream & os, Options & options, const Grammar & grammar) const
{
    // Returned value
    std::string replacement = checkedStringReplace(options.valueAsToken, Options::VAR_VALUE, Options::VAR_RETURN);
    replacement = checkedStringReplace(replacement, Options::VAR_TYPE, options.tokenType);
    os << replacement << " = " << options.tokenName << ';';

    // Shift
    os << ' ' << options.shiftToken;

    // Push returned value
    os << ' ' << checkedStringReplace(options.pushValue, Options::VAR_VALUE, Options::VAR_RETURN);
    if(item.nextState != nullptr)
        os << " return " << item.nextState->numState << ';' << std::endl;
    else
        os << " return " << options.errorState << ';' << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
std::string ParserState::checkedStringReplace(const std::string & str, const std::string & pattern, const std::string & replacement) const
{
    // Check that replacement string doesn't contains the pattern to avoid infinite loop
    if(replacement.find(pattern) != std::string::npos)
        return str;

    // Replace each occurrence
    std::string replacedStr(str);
    std::size_t pos = 0;

    while((pos = replacedStr.find(pattern, pos)) != std::string::npos)
        replacedStr.replace(pos, pattern.size(), replacement);

    return replacedStr;
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
