////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef LR1PARSER_H
#define LR1PARSER_H
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

#endif /* LR1PARSER_H */
