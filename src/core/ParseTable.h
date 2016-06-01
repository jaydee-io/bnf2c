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
#include "config/Options.h"
#include "Errors.h"

#include <list>
#include <cstdint>
#include <ostream>


class ParseTable
{
    public :
        typedef std::list<ParserState> States;

    public :
        ParseTable(const Grammar & grammar, Options & options);

        void generateStates(void);
        void check(void);

        void printDebug(std::ostream & os) const;

        const States & getStates(void) const;

    public :
        Errors<GeneratingError> errors;

    protected :
        const Grammar & m_grammar;
        Options &       m_options;

        States          m_states;
};

#endif /* _PARSINGTABLE_H_ */
