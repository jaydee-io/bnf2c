////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "generator/SwitchGenerator.h"

////////////////////////////////////////////////////////////////////////////////
SwitchGenerator::SwitchGenerator(Indenter & indenter, const std::string & switchOnExpr, const std::string & defaultCode)
: m_indenter(indenter), m_switchOnExpr(switchOnExpr), m_defaultCode(defaultCode)
{
}

////////////////////////////////////////////////////////////////////////////////
void SwitchGenerator::printBeginTo(std::ostream & os) const
{
    os << m_indenter << "switch(" << m_switchOnExpr << ")" << std::endl;
    os << m_indenter << "{" << std::endl;
    m_indenter++;
}

////////////////////////////////////////////////////////////////////////////////
void SwitchGenerator::printEndTo(std::ostream & os) const
{
    printDefaultTo(os);
    m_indenter--;
    os << m_indenter << "}" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void SwitchGenerator::printDefaultTo(std::ostream & os) const
{
    if(!m_defaultCode.empty())
        os << m_indenter << "default : " << m_defaultCode << std::endl;
}
