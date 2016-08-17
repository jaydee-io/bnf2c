////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _LR0PARSER_H_
#define _LR0PARSER_H_
#include "core/Parser.h"
#include "LR0State.h"

class LR0Parser : public Parser
{
    public :
        using Parser::Parser;

    protected :
        StatePtr createStartState(void) override;
        std::unordered_map<std::string, StatePtr> createSuccessorStates(const StatePtr & state) override;
};

#endif /* _LR0PARSER_H_ */
