////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _PARSER_H_
#define _PARSER_H_
#include "Grammar.h"
#include "ParserState.h"
#include "Symbol.h"
#include "config/Options.h"
#include "Errors.h"

#include <list>
#include <memory>
#include <cstdint>
#include <ostream>

class Parser
{
    public :
        using StatePtr = std::unique_ptr<ParserState>;
        using States   = std::list<StatePtr>;

    public :
        Parser(const Grammar & grammar, Options & options);

        void generateStates(void);
        void check(void);

        void printDebug(std::ostream & os) const;

        const States & getStates(void) const;

    public :
        Errors<GeneratingError> errors;

    protected :
        StatePtr & addNewState(StatePtr && state);
        StatePtr & addOrMergeState(StatePtr && state);

        virtual StatePtr createStartState(void) = 0;
        virtual std::unordered_map<std::string, StatePtr> createSuccessorStates(const StatePtr & state) = 0;

        template<typename StateType>
        StateType & getStateAs(std::unordered_map<std::string, Parser::StatePtr> & successors, const std::string & name)
        {
            auto & statePtr = successors[name];

            if(!statePtr)
                statePtr = std::make_unique<StateType>();

            return *reinterpret_cast<StateType *>(statePtr.get());
        }

        const Grammar & m_grammar;
        Options &       m_options;

        States          m_states;
};

#endif /* _PARSER_H_ */
