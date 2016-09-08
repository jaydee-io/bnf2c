////////////////////////////////////////////////////////////////////////////////
//ol isIntermediate(void) const { return type == Type::INTERMED
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "generator/StateGenerator.h"
#include <map>
#include <set>
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
    // If whatever the terminal the action is the same, don't generate a switch
    if(m_state.isSameActionForAllTerminals(m_grammar, m_options.endOfInputToken))
    {
        printReduceActionTo(m_state.items.front().rule, os);
    }
    else
    {
        // Regroup all cases of an item
        std::unordered_map<ParsingAction, std::unordered_set<std::string> > cases;
        for(const auto & terminal : m_grammar.terminals)
            cases[m_state.getAction(terminal, m_options.endOfInputToken)].insert(terminal);
        cases[m_state.getAction(m_options.endOfInputToken, m_options.endOfInputToken)].insert(m_options.endOfInputToken);

        // Switch on terminal
        m_switchOnTerminal.printBeginTo(os);
        for(const auto & casesOfItem : cases)
        {
            // Generate all cases
            if(casesOfItem.first.type != ParsingAction::Type::ERROR)
            {
                for(const auto & terminal : casesOfItem.second)
                {
                    os << m_options.indent << "case " << m_options.tokenPrefix << terminal << " : ";

                    if(casesOfItem.second.size() > 1)
                        os << std::endl;
                }
            }

            // Generate action
            switch(casesOfItem.first.type)
            {
                case ParsingAction::Type::SHIFT :
                    if(casesOfItem.second.size() == 1)
                        printShiftActionTo(casesOfItem.first.shiftNextState, os);
                    else
                    {
                        m_options.indent++;
                        os << m_options.indent;
                        printShiftActionTo(casesOfItem.first.shiftNextState, os);
                        m_options.indent--;
                    }
                    break;
                case ParsingAction::Type::REDUCE :
                    printReduceActionTo(*casesOfItem.first.reduceRule, os);
                    break;
                case ParsingAction::Type::ACCEPT :
                    os << "return " << m_options.acceptState << ";" << std::endl;
                    break;
                case ParsingAction::Type::ERROR :
                    /* Error case is handled at higher level */
                    break;
            }
        }
    }

    m_switchOnTerminal.printEndTo(os);
    os << m_options.indent << "break;" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void StateGenerator::printReduceActionTo(const Rule & reduceRule, std::ostream & os) const
{
    os << m_options.indent << '{' << std::endl;
    m_options.indent++;

    // Rule action code
    os << m_options.indent << m_options.valueType << ' ' << Vars::RETURN << ';' << std::endl << std::endl;

    if(reduceRule.action.find_first_of("\n\r") == std::string::npos)
        os << m_options.indent;
    os << reduceRule.action << std::endl << std::endl;

    // Values stack
    os << m_options.indent << m_options.popValues.replaceParam(Vars::NB_VALUES, std::to_string(reduceRule.symbols.size()))  << std::endl;
    os << m_options.indent << m_options.pushValue.replaceParam(Vars::VALUE,     Vars::RETURN)                       << std::endl;

    // States stack
    os << m_options.indent << m_options.popState.replaceParam(Vars::NB_STATES, std::to_string(reduceRule.symbols.size())) << std::endl;

    // New state
    if(m_options.useTableForBranches)
        os << m_options.indent << "return " << m_options.branchFunctionName << "[(" << m_grammar.intermediates.size() << "*" << m_options.topState << ") + " << m_grammar.getIntermediateIndex(reduceRule.name) << "];" << std::endl;
    else
        os << m_options.indent << "return " << m_options.branchFunctionName << "(" << m_grammar.getIntermediateIndex(reduceRule.name) << ");" << std::endl;

    m_options.indent--;
    os << m_options.indent << '}' << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void StateGenerator::printShiftActionTo(const ParserState * nextState, std::ostream & os) const
{
    // Push token
    os << m_options.pushValue.replaceParam(Vars::VALUE, m_options.tokenName);

    // Shift
    os << ' ' << m_options.shiftToken;

    if(nextState != nullptr)
        os << " return " << nextState->numState << ';' << std::endl;
    else
        os << " return " << m_options.errorState << ';' << std::endl;
}

