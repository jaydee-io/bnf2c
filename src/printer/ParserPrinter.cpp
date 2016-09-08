////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "PrettyPrinters.h"
#include "core/ParserState.h"
#include "core/Parser.h"
#include "core/Rule.h"
#include "core/Grammar.h"
#include "config/Options.h"

#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cstring>

#define CENTER(msg, maxSize)    std::setw((maxSize - ::strlen(msg)) / 2) << ' ' << msg << std::setw(maxSize - ((maxSize - ::strlen(msg)) / 2) - ::strlen(msg) - 1) << ' '

////////////////////////////////////////////////////////////////////////////////
template<typename Container>
std::string concatenateStrings(const Container & strings, const std::string & excluded = "", size_t minSize = 0)
{
    std::stringstream stream;

    for(const std::string & name : strings)
        if(name != excluded)
            stream << std::setw(std::max(name.length(), minSize)) << std::left << name << '|';

    return stream.str();
}

////////////////////////////////////////////////////////////////////////////////
void printStateActions(std::ostream & os, const ParsingAction & action, size_t padding)
{
    switch(action.type)
    {
        case ParsingAction::Type::SHIFT :  os << 'S' << std::setw(padding - 1) << std::left << (action.shiftNextState != nullptr ? action.shiftNextState->numState : -1) << '|'; break;
        case ParsingAction::Type::REDUCE : os << 'R' << std::setw(padding - 1) << std::left << (action.reduceRule != nullptr ? action.reduceRule->numRule : -1) << '|'; break;
        case ParsingAction::Type::ACCEPT : os << std::setw(padding) << std::left << "ACC" << '|'; break;
        case ParsingAction::Type::ERROR :  os << std::setw(padding + 1) << std::right << '|'; break;
    };
}

////////////////////////////////////////////////////////////////////////////////
void printStateBranches(std::ostream & os, const ParserState * nextState, size_t padding)
{
    if(nextState)
        os << std::setw(padding) << std::left << nextState->numState << '|';
    else
        os << std::setw(padding + 1) << std::right << '|';
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Parser & parser)
{
    // Header
    std::string strTerminals = concatenateStrings(parser.getGrammar().terminals) + parser.getOptions().endOfInputToken + '|';
    std::string strIntermediates = concatenateStrings(parser.getGrammar().intermediates, parser.getGrammar().START_RULE);

    os << "     |" << CENTER("Actions", std::max(strTerminals.length(), 7u)) << '|';
    os <<             CENTER("Branchs", std::max(strIntermediates.length(), 7u)) << '|' << std::endl;
    os << "State|" << strTerminals << strIntermediates << std::endl;
    os << std::setfill('-') << std::setw(strTerminals.length() + strIntermediates.length() + 6) << '-' << std::setfill(' ') << std::endl;

    // Table
    auto maxSizeIntermediate = std::to_string(parser.getStates().size()).length();
    auto eoit = parser.getOptions().endOfInputToken;
    for(const auto & state : parser.getStates())
    {
        os << std::left << std::setw(5) << state->numState << '|';

        // Action
        for(const auto & terminal : parser.getGrammar().terminals)
            printStateActions(os, state->getAction(terminal, eoit), terminal.length());
        printStateActions(os, state->getAction(eoit, eoit), eoit.length());

        // Goto
        for(const auto & intermediate : parser.getGrammar().intermediates)
            if(intermediate != parser.getGrammar().START_RULE)
                printStateBranches(os, state->getGoto(intermediate), std::max(intermediate.length(), maxSizeIntermediate));

        os << std::endl;
    }
    os << std::endl;

    // Print items sets
    for(const auto & state : parser.getStates())
        os << *state << std::endl;

    return os;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const ParserState & state)
{
    os << "Set " << state.numState << std::endl;

    for(Item item : state.items)
        os << item << std::endl;

    return os;
}

