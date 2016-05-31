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
    m_parseFunction(m_options.indent,  m_options.stateType, m_options.parseFunctionName, m_options.tokenType, m_options.tokenName, m_options.throwedExceptions, m_options.errorState),
    m_branchFunction(m_options.indent, m_options.stateType, m_options.branchFunctionName, m_options.intermediateType, "intermediate", "", m_options.errorState),
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
    m_parseFunction.printBeginTo(os);

    // Returned value
    os << m_options.indent << m_options.valueType << ' ' << Options::VAR_RETURN << ';' << std::endl;

    // Switch on state
    m_switchOnStates.printBeginTo(os);
    for(const auto & generator : m_stateGenerators)
        generator.printActionsTo(os);
    m_switchOnStates.printEndTo(os);

    m_parseFunction.printEndTo(os);
}

////////////////////////////////////////////////////////////////////////////////
void ParserGenerator::printBranchSwitchTo(std::ostream & os) const
{
    m_branchFunction.printBeginTo(os);

    m_switchOnStates.printBeginTo(os);
    for(const auto & generator : m_stateGenerators)
        generator.printBranchesSwitchTo(os);
    m_switchOnStates.printEndTo(os);

    m_branchFunction.printEndTo(os);
}

////////////////////////////////////////////////////////////////////////////////
void ParserGenerator::printBranchTableTo(std::ostream & os) const
{
    os << m_options.indent << "const " << m_options.stateType << " " << m_options.branchFunctionName << "[] = {" << std::endl;
    m_options.indent++;
    bool firstGenerator = true;
    for(const auto & generator : m_stateGenerators)
    {
        if(!firstGenerator)
            os << ", " << std::endl;

        generator.printBranchesTableTo(os);
        firstGenerator = false;
    }
    os << std::endl;
    m_options.indent--;
    os << m_options.indent << "};" << std::endl;
}
