////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "ParsingAction.h"

////////////////////////////////////////////////////////////////////////////////
bool ParsingAction::operator ==(const ParsingAction & action) const
{
    if(type != action.type)
        return false;

    switch(type)
    {
        case ParsingAction::Type::SHIFT  :
            return shiftNextState == action.shiftNextState;
            break;
        case ParsingAction::Type::REDUCE :
            return reduceRule == action.reduceRule;
            break;
        case ParsingAction::Type::ACCEPT :
        case ParsingAction::Type::ERROR  :
            return true;
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
bool ParsingAction::operator !=(const ParsingAction & action) const
{
    return !(*this == action);
}

