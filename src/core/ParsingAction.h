////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef PARSING_ACTION_H
#define PARSING_ACTION_H
#include <functional>

class Rule;
class ParserState;

struct ParsingAction
{
    enum class Type
    {
        SHIFT,
        REDUCE,
        ACCEPT,
        ERROR
    } type;

    union 
    {
        const Rule * reduceRule;
        const ParserState * shiftNextState;
    };

    bool operator ==(const ParsingAction & action) const;
    bool operator !=(const ParsingAction & action) const;
};

namespace std {

template <> 
struct hash<ParsingAction>
{
    size_t operator()(const ParsingAction & action) const
    {
        return ((size_t) action.type) ^ ((size_t) action.reduceRule);
    }
};

}

#endif /* PARSING_ACTION_H */

