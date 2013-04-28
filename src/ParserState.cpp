////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013, Jerome DUMESNIL
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by Jerome DUMESNIL.
// 4. Neither the name of the Jerome DUMESNIL nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY JEROME DUMESNIL ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL JEROME DUMESNIL BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////
#include "ParserState.h"

#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
bool Item::operator ==(const Item & item) const
{
    if(&item == this)
        return true;

    return (dot == item.dot) && (rule == item.rule);
}

////////////////////////////////////////////////////////////////////////////////
Item::Type Item::getType(void) const
{
    if(dot >= rule.symbols.size())
        return Type::REDUCE;
    else
        return Type::SHIFT;
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::addRule(const Rule & rule, const SymbolIterator dot)
{
    items.push_back(Item({rule, dot, nullptr}));
}

////////////////////////////////////////////////////////////////////////////////
bool ParserState::contains(const Item & item) const
{
    return std::find(items.begin(), items.end(), item) != items.end();
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::close(const Grammar & grammar)
{
    if(!m_isClosed)
    {
        std::unordered_set<Dictionnary::Index>  symbolsClosed;

        for(ItemList::iterator it = items.begin(); it != items.end(); ++it)
        {
            if(it->dot < it->rule.symbols.size())
            {
                const Symbol & symbol = it->rule.symbols[it->dot];

                if(symbol.type == Symbol::Type::INTERMEDIATE)
                {
                    if(symbolsClosed.find(symbol.name) == symbolsClosed.end())
                    {
                        symbolsClosed.insert(symbol.name);

                        Grammar::RuleRange range = grammar[symbol.name];
                        for(Grammar::RuleIterator ruleIt = range.first; ruleIt != range.second; ++ruleIt)
                            addRule(ruleIt->second);
                    }
                }
            }
        }

        m_isClosed = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::outputActions(OutputFormatter & of, const Grammar & grammar) const
{
    of.outputStream << of.indent << "case " << numState << " :" << std::endl;

    of.indent++;
    outputActionItems(of, grammar);
    of.indent--;
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::outputBranchesSwitch(OutputFormatter & of, const Grammar & grammar) const
{
    std::unordered_set<std::string> outCases;
    of.indent++++;
    for(const Item & item : items)
    {
        std::stringstream os;

        if((item.dot < item.rule.symbols.size()) && (item.rule.symbols[item.dot].type == Symbol::Type::INTERMEDIATE))
        {
            os << of.indent << "case " << grammar.intermediates.index(item.rule.symbols[item.dot].name) << " : ";
            if(item.nextState != nullptr)
                os << "return " << item.nextState->numState << "; ";
            else
                os << "return " << of.errorState << "; ";
            os << "break;" << std::endl;
            outCases.insert(os.str());
        }
    }
    of.indent----;

    if(!outCases.empty())
    {
        // Epilog
        of.outputStream << of.indent << "case " << numState << " :";
        of.indent++;

        of.outputStream << std::endl;
        of.outputStream << of.indent << "switch(" << of.intermediateName << ")" << std::endl;
        of.outputStream << of.indent << "{" << std::endl;
        for(const std::string & str : outCases)
            of.outputStream << str;

        if(of.defaultSwitchStatement)
        {
            of.indent++;
            of.outputStream << of.indent << "default : return " << of.errorState << "; break;" << std::endl;
            of.indent--;
        }

        of.outputStream << of.indent << "}" << std::endl;
        of.indent--;
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::outputBranchesTable(OutputFormatter & of, const Grammar & grammar) const
{
    of.outputStream << of.indent;
    for(Dictionnary::Index itIntermediate = grammar.intermediates.begin(); itIntermediate != grammar.intermediates.end(); ++itIntermediate)
    {
        if(itIntermediate != grammar.intermediates.begin())
            of.outputStream << ", ";

        ItemList::const_iterator itItem;
        for(itItem = items.begin(); itItem != items.end(); ++itItem)
            if((itItem->dot < itItem->rule.symbols.size()) && (itItem->rule.symbols[itItem->dot].name == itIntermediate))
                break;

        if(itItem != items.end() && (itItem->nextState != nullptr))
            of.outputStream << itItem->nextState->numState;
        else
            of.outputStream << of.errorState;
    }
}

////////////////////////////////////////////////////////////////////////////////
bool ParserState::operator ==(const ParserState & set) const
{
    if(&set == this)
        return true;

    return items == set.items;
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::outputActionItems(OutputFormatter & of, const Grammar & grammar) const
{
    // Reduce
    if((items.size() == 1) && (items.begin()->getType() == Item::Type::REDUCE))
    {
        const Item & item = *items.begin();

        if(!item.rule.action.empty())
        {
            of.outputStream << of.indent << '{' << std::endl;
            of.outputStream << item.rule.action << std::endl;
            of.outputStream << of.indent << '}' << std::endl;
        }
        outputPopFunction(of, item.rule.symbols.size());
        if(of.useTableForBranches)
            of.outputStream << " return " << of.branchFunctionName << "[(" << grammar.intermediates.size() << "*" << of.topState << ") + " << grammar.intermediates.index(item.rule.name) << "]; break;" << std::endl;
        else
            of.outputStream << " return " << of.branchFunctionName << "(" << grammar.intermediates.index(item.rule.name) << "); break;" << std::endl;

        return;
    }

    std::unordered_set<std::string> outCases;
    of.indent++;
    for(const Item & item : items)
    {
        std::stringstream os;

        // Accept
        if((item.rule == grammar.getStartRule()) && (item.dot >= item.rule.symbols.size()))
        {
            os << of.indent << "case " << of.tokenPrefix << of.endOfInputToken << " : return " << of.acceptState << "; break;" << std::endl;
            outCases.insert(os.str());
            continue;
        }

        // Shift
        if((item.dot < item.rule.symbols.size()) && (item.rule.symbols[item.dot].type == Symbol::Type::TERMINAL))
        {
            os << of.indent << "case " << of.tokenPrefix << item.rule.symbols[item.dot] << " :";
            os << " " << of.shiftToken << ';';
            if(item.nextState != nullptr)
                os << " return " << item.nextState->numState << ";";
            else
                os << " return " << of.errorState << ";";
            os << " break;" << std::endl;
            outCases.insert(os.str());
            continue;
        }
    }
    of.indent--;

    if(!outCases.empty())
    {
        of.outputStream << of.indent << "switch(" << of.tokenName << ")" << std::endl;
        of.outputStream << of.indent << "{" << std::endl;
        for(const std::string & str : outCases)
            of.outputStream << str;
        if(of.defaultSwitchStatement)
        {
            of.indent++;
            of.outputStream << of.indent << "default : return " << of.errorState << "; break;" << std::endl;
            of.indent--;
        }
        of.outputStream << of.indent << "}" << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::outputPopFunction(OutputFormatter & of, int nbStates) const
{
    std::string popFunction(of.popState);

    of.outputStream << of.indent << popFunction.replace(popFunction.find(OutputFormatter::VAR_NB_STATES), OutputFormatter::VAR_NB_STATES.size(), std::to_string(nbStates)) << ";";
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Item & item)
{
    os << "<" << item.rule.name << "> ::=";

    for(SymbolIterator it = 0; it < item.rule.symbols.size(); ++it)
    {
        if(it == item.dot)
            os << " •" << item.rule.symbols[it];
        else
            os << " " << item.rule.symbols[it];
    }

    if(item.dot == item.rule.symbols.size())
        os << " •";

    return os;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const ParserState & itemSet)
{
    os << "Set " << itemSet.numState << std::endl;

    for(Item item : itemSet.items)
        os << item << std::endl;

    return os;
}
