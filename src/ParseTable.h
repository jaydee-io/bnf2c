////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _PARSINGTABLE_H_
#define _PARSINGTABLE_H_
#include "Grammar.h"
#include "ParserState.h"
#include "Options.h"
#include "Errors.h"

#include <list>
#include <cstdint>
#include <ostream>


class ParseTable
{
    protected :
        typedef std::list<ParserState> States;

    public :
        ParseTable(const Grammar & grammar, Options & options);

        void generateStates(void);
        void check(void);

        void generateBranchesCode(std::ostream & os) const;
        void generateParseCode(std::ostream & os) const;

        void printDebug(std::ostream & os) const;

    protected :
        void generateBranchSwitch(std::ostream & os) const;
        void generateBranchTable (std::ostream & os) const;

    public :
        Errors<GeneratingError> errors;

    protected :
        const Grammar & m_grammar;
        Options &       m_options;

        States          m_states;
};

#endif /* _PARSINGTABLE_H_ */
