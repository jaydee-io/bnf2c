////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "generator/ParserGenerator.h"

////////////////////////////////////////////////////////////////////////////////
ParserGenerator::ParserGenerator(const ParseTable & table, const Grammar & grammar, Options & options)
: m_options(options),
    m_switchOnStates(m_options.indent, m_options.topState, m_options.defaultSwitchStatement ? "return " + m_options.errorState + ";" : "")
{
    m_stateGenerators.reserve(table.getStates().size());
    for(const ParserState & state : table.getStates())
        m_stateGenerators.emplace_back(state, grammar, options);
}

////////////////////////////////////////////////////////////////////////////////
void ParserGenerator::printTo(std::ostream & os) const
{
    printBranchesCodeTo(os);
    os << std::endl;
    printParseCodeTo(os);
}

////////////////////////////////////////////////////////////////////////////////
void ParserGenerator::printBranchesCodeTo(std::ostream & os) const
{
    // Branches
    if(m_options.useTableForBranches)
        printBranchTableTo(os);
    else
        printBranchSwitchTo(os);
}

////////////////////////////////////////////////////////////////////////////////
void ParserGenerator::printParseCodeTo(std::ostream & os) const
{
    // Function prototype
    printFunctionBeginTo(os, m_options.parseFunctionName, m_options.tokenType, m_options.tokenName, m_options.throwedExceptions);

    // Returned value
    os << m_options.indent << m_options.valueType << ' ' << Options::VAR_RETURN << ';' << std::endl;

    // Switch on state
    m_switchOnStates.printBeginTo(os);
    for(const StateGenerator & generator : m_stateGenerators)
        generator.printActionsTo(os);
    m_switchOnStates.printEndTo(os);

    // Return error
    os << m_options.indent << "return " << m_options.errorState << ';' << std::endl;

    printFunctionEndTo(os);
}

////////////////////////////////////////////////////////////////////////////////
void ParserGenerator::printBranchSwitchTo(std::ostream & os) const
{
    os << m_options.indent << m_options.stateType << " " << m_options.branchFunctionName << "(" << m_options.intermediateType << " intermediate)" << std::endl;
    os << m_options.indent << '{' << std::endl;
    m_options.indent++;
    m_switchOnStates.printBeginTo(os);
    for(const StateGenerator & generator : m_stateGenerators)
        generator.printBranchesSwitchTo(os);
    m_switchOnStates.printEndTo(os);
    os << m_options.indent << "return " << m_options.errorState << ';' << std::endl;

    printFunctionEndTo(os);
}

////////////////////////////////////////////////////////////////////////////////
void ParserGenerator::printBranchTableTo(std::ostream & os) const
{
    os << m_options.indent << "const " << m_options.stateType << " " << m_options.branchFunctionName << "[] = {" << std::endl;
    m_options.indent++;
    for(std::vector<StateGenerator>::const_iterator it = m_stateGenerators.begin(); it != m_stateGenerators.end(); ++it)
    {
        if(it != m_stateGenerators.begin())
            os << ", " << std::endl;

        it->printBranchesTableTo(os);
    }
    os << std::endl;
    m_options.indent--;
    os << m_options.indent << "};" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void ParserGenerator::printFunctionBeginTo(std::ostream & os, const std::string & funcName, const std::string & paramType, const std::string & paramName, const std::string & exceptions) const
{
    os << m_options.indent << m_options.stateType << " " << funcName << "(const " << paramType << ' ' << paramName << ')';
    if(!exceptions.empty())
        os << " throw(" << exceptions << ")";
    os << std::endl;
    os << m_options.indent << '{' << std::endl;
    m_options.indent++;
}

////////////////////////////////////////////////////////////////////////////////
void ParserGenerator::printFunctionEndTo(std::ostream & os) const
{
    m_options.indent--;
    os << m_options.indent << "}" << std::endl;
}
