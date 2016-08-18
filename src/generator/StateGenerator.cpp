////////////////////////////////////////////////////////////////////////////////
//ol isIntermediate(void) const { return type == Type::INTERMED
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
: m_state(state), m_grammar(grammar), m_options(options),
    m_switchOnIntermediate(m_options.indent, m_options.intermediateName, m_options.defaultSwitchStatement ? "return " + m_options.errorState + ";" : ""), 
    m_switchOnTerminal(m_options.indent, m_options.getTypeOfToken.replaceParam(Vars::TOKEN, m_options.tokenName).toString(), m_options.defaultSwitchStatement ? "return " + m_options.errorState + ";" : "")
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

        if(!item.isDotAtEnd() && item.dottedSymbol->isIntermediate())
        {
            os << m_options.indent << "case " << m_grammar.getIntermediateIndex(item.dottedSymbol->name) << " : ";
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
        m_switchOnIntermediate.printBeginTo(os);
        for(const std::string & str : outCases)
            os << str;
        m_switchOnIntermediate.printEndTo(os);

        os << m_options.indent << "break;" << std::endl;
        m_options.indent--;
    }
}

////////////////////////////////////////////////////////////////////////////////
void StateGenerator::printBranchesTableTo(std::ostream & os) const
{
    os << m_options.indent;
    for(const auto & intermediate : m_grammar.intermediates)
    {
        if(intermediate != *m_grammar.intermediates.begin())
            os << ", ";

        ParserState::ItemList::const_iterator itItem;
        for(itItem = m_state.items.begin(); itItem != m_state.items.end(); ++itItem)
            if(!itItem->isDotAtEnd() && itItem->dottedSymbol->name == intermediate)
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
    auto & firstItem = m_state.items.front();
    if(m_state.items.size() >= 1 && firstItem.isReduce() && firstItem.rule.numRule > 1)
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
            if(it->isShift() && it->dottedSymbol->name == terminal)
            {
                cases[*it].insert(terminal);
                break;
            }
        }

        // Reduce
        if(it == m_state.items.end() && firstItem.isReduce() && firstItem.rule.numRule > 1)
            cases[firstItem].insert(terminal);
    }

    // If there is a reduce rule (not the accept one), reduce also when current token is the end of input
    if(firstItem.isReduce() && firstItem.rule.numRule > 1)
        cases[firstItem].insert(m_options.endOfInputToken);

    // Switch on terminal
    m_switchOnTerminal.printBeginTo(os);

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
        if(casesOfItem.first.isShift() && casesOfItem.first.dottedSymbol->isTerminal())
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
        else if(casesOfItem.first.isReduce())
        {
            os << std::endl;
            printReduceActionTo(casesOfItem.first, os);
        }
    }

    // Accept rule
    if(firstItem.isReduce() && firstItem.rule.numRule == 1)
        os << m_options.indent << "case " << m_options.tokenPrefix << m_options.endOfInputToken << " : return " << m_options.acceptState << ";" << std::endl;

    m_switchOnTerminal.printEndTo(os);
    os << m_options.indent << "break;" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void StateGenerator::printReduceActionTo(const Item & item, std::ostream & os) const
{
    os << m_options.indent << '{' << std::endl;
    m_options.indent++;

    // Rule action code
    os << m_options.indent << m_options.valueType << ' ' << Vars::RETURN << ';' << std::endl << std::endl;

    if(item.rule.action.find_first_of("\n\r") == std::string::npos)
        os << m_options.indent;
    os << item.rule.action << std::endl << std::endl;

    // Values stack
    os << m_options.indent << m_options.popValues.replaceParam(Vars::NB_VALUES, std::to_string(item.rule.symbols.size()))  << std::endl;
    os << m_options.indent << m_options.pushValue.replaceParam(Vars::VALUE,     Vars::RETURN)                       << std::endl;

    // States stack
    os << m_options.indent << m_options.popState.replaceParam(Vars::NB_STATES, std::to_string(item.rule.symbols.size())) << std::endl;

    // New state
    if(m_options.useTableForBranches)
        os << m_options.indent << "return " << m_options.branchFunctionName << "[(" << m_grammar.intermediates.size() << "*" << m_options.topState << ") + " << m_grammar.getIntermediateIndex(item.rule.name) << "];" << std::endl;
    else
        os << m_options.indent << "return " << m_options.branchFunctionName << "(" << m_grammar.getIntermediateIndex(item.rule.name) << ");" << std::endl;

    m_options.indent--;
    os << m_options.indent << '}' << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void StateGenerator::printShiftActionTo(const Item & item, std::ostream & os) const
{
    // Push token
    os << m_options.pushValue.replaceParam(Vars::VALUE, m_options.tokenName);

    // Shift
    os << ' ' << m_options.shiftToken;

    if(item.nextState != nullptr)
        os << " return " << item.nextState->numState << ';' << std::endl;
    else
        os << " return " << m_options.errorState << ';' << std::endl;
}

