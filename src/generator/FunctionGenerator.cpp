////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "generator/FunctionGenerator.h"

////////////////////////////////////////////////////////////////////////////////
FunctionGenerator::FunctionGenerator(Indenter & indenter, const std::string & returnType, const std::string & funcName, const std::string & paramType, const std::string & paramName, const std::string & exceptions, const std::string & returnExpr)
: m_indenter(indenter), m_returnType(returnType), m_funcName(funcName), m_paramType(paramType), m_paramName(paramName), m_exceptions(exceptions), m_returnExpr(returnExpr)
{
}

////////////////////////////////////////////////////////////////////////////////
void FunctionGenerator::printBeginTo(std::ostream & os) const
{
    os << m_indenter << m_returnType << " " << m_funcName << "(const " << m_paramType << ' ' << m_paramName << ')';
    if(!m_exceptions.empty())
        os << " throw(" << m_exceptions << ")";
    os << std::endl;
    os << m_indenter << '{' << std::endl;
    m_indenter++;
}

////////////////////////////////////////////////////////////////////////////////
void FunctionGenerator::printEndTo(std::ostream & os) const
{
    os << m_indenter << "return " << m_returnExpr << ';' << std::endl;
    m_indenter--;
    os << m_indenter << "}" << std::endl;
}
