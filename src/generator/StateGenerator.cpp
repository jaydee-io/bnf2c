////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "generator/StateGenerator.h"
#include <map>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
StateGenerator::StateGenerator(const ParserState & state, const Grammar & grammar, Options & options)
: m_state(state), m_grammar(grammar), m_options(options)
{
}

////////////////////////////////////////////////////////////////////////////////
void StateGenerator::printActionsTo(std::ostream & os) const
{
    os << m_options.indent << "case " << m_state.numState << " :" << std::endl;

    m_options.indent++;
    printActionItemsTo(os);
    m_options.indent--;
}

////////////////////////////////////////////////////////////////////////////////
void StateGenerator::printBranchesSwitchTo(std::ostream & os) const
{
    std::unordered_set<std::string> outCases;
    m_options.indent++++;
    for(const Item & item : m_state.items)
    {
        std::stringstream os;

        if((item.dot < item.rule.symbols.size()) && (item.rule.symbols[item.dot].type == Symbol::Type::INTERMEDIATE))
        {
            os << m_options.indent << "case " << m_grammar.intermediates.index(item.rule.symbols[item.dot].name) << " : ";
            if(item.nextState != nullptr)
                os << "return " << item.nextState->numState << ";" << std::endl;
            else
                os << "return " << m_options.errorState << ";" << std::endl;
            outCases.insert(os.str());
        }
    }
    m_options.indent----;

    if(!outCases.empty())
    {
        // Epilog
        os << m_options.indent << "case " << m_state.numState << " :";
        m_options.indent++;

        os << std::endl;
        os << m_options.indent << "switch(" << m_options.intermediateName << ")" << std::endl;
        os << m_options.indent << "{" << std::endl;
        for(const std::string & str : outCases)
            os << str;

        if(m_options.defaultSwitchStatement)
        {
            m_options.indent++;
            os << m_options.indent << "default : return " << m_options.errorState << ";" << std::endl;
            m_options.indent--;
        }

        os << m_options.indent << "}" << std::endl;
        os << m_options.indent << "break;" << std::endl;
        m_options.indent--;
    }
}

////////////////////////////////////////////////////////////////////////////////
void StateGenerator::printBranchesTableTo(std::ostream & os) const
{
    os << m_options.indent;
    for(Dictionary::const_iterator itIntermediate = m_grammar.intermediates.begin(); itIntermediate != m_grammar.intermediates.end(); ++itIntermediate)
    {
        if(itIntermediate != m_grammar.intermediates.begin())
            os << ", ";

        ParserState::ItemList::const_iterator itItem;
        for(itItem = m_state.items.begin(); itItem != m_state.items.end(); ++itItem)
            if((itItem->dot < itItem->rule.symbols.size()) && (itItem->rule.symbols[itItem->dot].name == *itIntermediate))
                break;

        if(itItem != m_state.items.end() && (itItem->nextState != nullptr))
            os << itItem->nextState->numState;
        else
            os << m_options.errorState;
    }
}

////////////////////////////////////////////////////////////////////////////////
void StateGenerator::printActionItemsTo(std::ostream & os) const
{
    // If the first action is a reduce (not the accept one) whatever the terminal, don't generate a switch
    if((m_state.items.size() >= 1) && (m_state.items.begin()->getType() == Item::ActionType::REDUCE) && !m_state.isAnAcceptRule())
    {
        printReduceActionTo(*m_state.items.begin(), os);
        return;
    }

    // Regroup all cases of an item
    std::map<Item, std::set<std::string> > cases;
    for(const std::string & terminal : m_grammar.terminals)
    {
        ParserState::ItemList::const_iterator it;

        // Shift rule
        for(it = m_state.items.begin(); it != m_state.items.end(); ++it)
        {
            if((it->getType() == Item::ActionType::SHIFT) && (it->rule.symbols[it->dot].name == terminal))
            {
                cases[*it].insert(terminal);
                break;
            }
        }

        // Reduce
        if(it == m_state.items.end() && m_state.getReduceRule())
            cases[*m_state.getReduceRule()].insert(terminal);
    }

    // If there is a reduce rule (not the accept one), reduce also when current token is the end of input
    if(!m_state.isAnAcceptRule() && m_state.getReduceRule())
        cases[*m_state.getReduceRule()].insert(m_options.endOfInputToken);

    // Switch on terminal
    os << m_options.indent << "switch(" << checkedStringReplace(m_options.getTypeOfToken, Options::VAR_TOKEN, m_options.tokenName) << ")" << std::endl;
    os << m_options.indent << "{" << std::endl;
    m_options.indent++;

    for(const std::pair<const Item, std::set<std::string> > & casesOfItem : cases)
    {
        // Generate all cases
        for(std::set<std::string>::const_iterator it = casesOfItem.second.begin(); it != casesOfItem.second.end(); ++it)
        {
            os << m_options.indent << "case " << m_options.tokenPrefix << *it << " : ";

            if(casesOfItem.second.size() > 1)
                os << std::endl;
        }

        // Shift action
        if((casesOfItem.first.getType() == Item::ActionType::SHIFT) && (casesOfItem.first.rule.symbols[casesOfItem.first.dot].type == Symbol::Type::TERMINAL))
        {
            if(casesOfItem.second.size() == 1)
                printShiftActionTo(casesOfItem.first, os);
            else
            {
                m_options.indent++;
                os << m_options.indent;
                printShiftActionTo(casesOfItem.first, os);
                m_options.indent--;
            }
        }
        // Reduce action
        else if(casesOfItem.first.getType() == Item::ActionType::REDUCE)
        {
            os << std::endl;
            printReduceActionTo(casesOfItem.first, os);
        }
    }

    // Accept rule
    if(m_state.isAnAcceptRule())
        os << m_options.indent << "case " << m_options.tokenPrefix << m_options.endOfInputToken << " : return " << m_options.acceptState << ";" << std::endl;

    // Default switch statement
    if(m_options.defaultSwitchStatement)
        os << m_options.indent << "default : return " << m_options.errorState << ";" << std::endl;

    m_options.indent--;
    os << m_options.indent << "}" << std::endl;
    os << m_options.indent << "break;" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void StateGenerator::printReduceActionTo(const Item & item, std::ostream & os) const
{
    os << m_options.indent << '{' << std::endl;
    m_options.indent++;

    // No action specified by user, assume "$$ = $1"
    if(item.rule.action.empty())
    {
        std::string replacement = checkedStringReplace(m_options.getValue, Options::VAR_VALUE_IDX, std::to_string(item.rule.symbols.size() - 1));
        replacement = checkedStringReplace(replacement, Options::VAR_TYPE, m_grammar.getIntermediateType(item.rule.name));
        os << m_options.indent  << Options::VAR_RETURN << " = " << replacement << ';' << std::endl;
    }
    // Rule action code
    else
    {
        if(item.rule.action.find_first_of("\n\r") != std::string::npos)
            os << item.rule.action << std::endl << std::endl;
        else
            os << m_options.indent << item.rule.action << std::endl << std::endl;
    }

    // Values stack
    os << m_options.indent << checkedStringReplace(m_options.popValues, Options::VAR_NB_VALUES, std::to_string(item.rule.symbols.size()))  << std::endl;
    os << m_options.indent << checkedStringReplace(m_options.pushValue, Options::VAR_VALUE,     Options::VAR_RETURN)                       << std::endl;

    // States stack
    os << m_options.indent << checkedStringReplace(m_options.popState, Options::VAR_NB_STATES, std::to_string(item.rule.symbols.size())) << std::endl;

    // New state
    if(m_options.useTableForBranches)
        os << m_options.indent << "return " << m_options.branchFunctionName << "[(" << m_grammar.intermediates.size() << "*" << m_options.topState << ") + " << m_grammar.intermediates.index(item.rule.name) << "];" << std::endl;
    else
        os << m_options.indent << "return " << m_options.branchFunctionName << "(" << m_grammar.intermediates.index(item.rule.name) << ");" << std::endl;

    m_options.indent--;
    os << m_options.indent << '}' << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void StateGenerator::printShiftActionTo(const Item & item, std::ostream & os) const
{
    // Returned value
    std::string replacement = checkedStringReplace(m_options.valueAsToken, Options::VAR_VALUE, Options::VAR_RETURN);
    replacement = checkedStringReplace(replacement, Options::VAR_TYPE, m_options.tokenType);
    os << replacement << " = " << m_options.tokenName << ';';

    // Shift
    os << ' ' << m_options.shiftToken;

    // Push returned value
    os << ' ' << checkedStringReplace(m_options.pushValue, Options::VAR_VALUE, Options::VAR_RETURN);
    if(item.nextState != nullptr)
        os << " return " << item.nextState->numState << ';' << std::endl;
    else
        os << " return " << m_options.errorState << ';' << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
std::string StateGenerator::checkedStringReplace(const std::string & str, const std::string & pattern, const std::string & replacement) const
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
