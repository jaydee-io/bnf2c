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
#include <cstdint>
#include <ostream>

class Parser
{
    public :
        using States = std::list<ParserState::Ptr>;

    public :
        Parser(const Grammar & grammar, Options & options);

        void generateStates(void);
        void check(void);

        void printDebug(std::ostream & os) const;

        const States & getStates(void) const;

    public :
        Errors<GeneratingError> errors;

    protected :
        ParserState::Ptr & addNewState(ParserState::Ptr && state);
        ParserState::Ptr & addOrMergeState(ParserState::Ptr && state);

        virtual ParserState::Ptr createStartState(void) = 0;
        virtual std::unordered_map<std::string, ParserState::Ptr> createSuccessorStates(const ParserState::Ptr & state) = 0;

        template<typename StateType>
        static StateType & fetchOrInsertState(std::unordered_map<std::string, ParserState::Ptr> & successors, const std::string & name)
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