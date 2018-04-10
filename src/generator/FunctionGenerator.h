////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef FUNCTION_GENERATOR_H
#define FUNCTION_GENERATOR_H
#include "config/Options.h"
#include <ostream>

class FunctionGenerator
{
    public :
        FunctionGenerator(Indenter & indenter, const std::string & returnType, const std::string & funcName, const std::string & paramType, const std::string & paramName, const std::string & exceptions, const std::string & returnExpr);

        void printBeginTo(std::ostream & os) const;
        void printEndTo(std::ostream & os) const;

    private :
        Indenter &        m_indenter;
        const std::string m_returnType;
        const std::string m_funcName;
        const std::string m_paramType;
        const std::string m_paramName;
        const std::string m_exceptions;
        const std::string m_returnExpr;
};

#endif /* FUNCTION_GENERATOR_H */

