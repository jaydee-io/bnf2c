////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _STATE_GENERATOR_H_
#define _STATE_GENERATOR_H_
#include "core/ParserState.h"
#include "core/Grammar.h"
#include "config/Options.h"
#include "generator/SwitchGenerator.h"

#include <ostream>

class StateGenerator
{
    public :
        StateGenerator(const ParserState & state, const Grammar & grammar, Options & options);

        void printActionsTo       (std::ostream & os) const;
        void printBranchesSwitchTo(std::ostream & os) const;
        void printBranchesTableTo (std::ostream & os) const;

    private :
        void printActionItemsTo (std::ostream & os) const;
        void printReduceActionTo(const Item & item, std::ostream & os) const;
        void printShiftActionTo (const Item & item, std::ostream & os) const;

    private :
        const ParserState & m_state;
        const Grammar &     m_grammar;
        Options &           m_options;
        SwitchGenerator     m_switchOnIntermediate;
        SwitchGenerator     m_switchOnTerminal;
};

#endif /* _STATES_GENERATOR_H_ */

