////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _LR1PARSER_H_
#define _LR1PARSER_H_
#include "core/Parser.h"
#include "LR1State.h"

class LR1Parser : public Parser
{
    public :
        using Parser::Parser;

    protected :
        ParserState::Ptr createStartState(void) override;
        std::unordered_map<std::string, ParserState::Ptr> createSuccessorStates(const ParserState::Ptr & state) override;
};

#endif /* _LR1PARSER_H_ */
