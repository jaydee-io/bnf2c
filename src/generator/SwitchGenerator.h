////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef SWITCH_GENERATOR_H
#define SWITCH_GENERATOR_H
#include "config/Options.h"
#include <ostream>

class SwitchGenerator
{
    public :
        SwitchGenerator(Indenter & indenter, const std::string & switchOnExpr, const std::string & defaultCode);

        void printBeginTo(std::ostream & os) const;
        void printEndTo(std::ostream & os) const;

    private :
        void printDefaultTo(std::ostream & os) const;

        Indenter &        m_indenter;
        const std::string m_switchOnExpr;
        const std::string m_defaultCode;
};

#endif /* SWITCH_GENERATOR_H */

