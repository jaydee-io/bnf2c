////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _PARSER_GENERATOR_H_
#define _PARSER_GENERATOR_H_
#include "core/ParseTable.h"
#include "core/Grammar.h"
#include "config/Options.h"
#include "generator/StateGenerator.h"
#include "generator/SwitchGenerator.h"
#include "generator/FunctionGenerator.h"

#include <vector>
#include <ostream>

class ParserGenerator
{
    public :
        ParserGenerator(const ParseTable & table, const Grammar & grammar, Options & options);

        void printTo(std::ostream & os) const;

    private :

        void printBranchesCodeTo(std::ostream & os) const;
        void printParseCodeTo   (std::ostream & os) const;
        void printBranchSwitchTo(std::ostream & os) const;
        void printBranchTableTo (std::ostream & os) const;

    private :
        std::vector<StateGenerator> m_stateGenerators;
        Options &                   m_options;

        FunctionGenerator           m_parseFunction;
        FunctionGenerator           m_branchFunction;
        SwitchGenerator             m_switchOnStates;
};

#endif /* _PARSER_GENERATOR_H_ */

