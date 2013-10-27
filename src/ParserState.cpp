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
#include <iomanip>

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
        std::unordered_set<std::string>  symbolsClosed;

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
void ParserState::generateActions(std::ostream & os, Options & options, const Grammar & grammar) const
{
    os << options.indent << "case " << numState << " :" << std::endl;

    options.indent++;
    generateActionItems(os, options, grammar);
    options.indent--;
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::generateBranchesSwitch(std::ostream & os, Options & options, const Grammar & grammar) const
{
    std::unordered_set<std::string> outCases;
    options.indent++++;
    for(const Item & item : items)
    {
        std::stringstream os;

        if((item.dot < item.rule.symbols.size()) && (item.rule.symbols[item.dot].type == Symbol::Type::INTERMEDIATE))
        {
            os << options.indent << "case " << grammar.intermediates.index(item.rule.symbols[item.dot].name) << " : ";
            if(item.nextState != nullptr)
                os << "return " << item.nextState->numState << "; ";
            else
                os << "return " << options.errorState << "; ";
            os << "break;" << std::endl;
            outCases.insert(os.str());
        }
    }
    options.indent----;

    if(!outCases.empty())
    {
        // Epilog
        os << options.indent << "case " << numState << " :";
        options.indent++;

        os << std::endl;
        os << options.indent << "switch(" << options.intermediateName << ")" << std::endl;
        os << options.indent << "{" << std::endl;
        for(const std::string & str : outCases)
            os << str;

        if(options.defaultSwitchStatement)
        {
            options.indent++;
            os << options.indent << "default : return " << options.errorState << "; break;" << std::endl;
            options.indent--;
        }

        os << options.indent << "}" << std::endl;
        options.indent--;
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::generateBranchesTable(std::ostream & os, Options & options, const Grammar & grammar) const
{
    os << options.indent;
    for(Dictionary::const_iterator itIntermediate = grammar.intermediates.begin(); itIntermediate != grammar.intermediates.end(); ++itIntermediate)
    {
        if(itIntermediate != grammar.intermediates.begin())
            os << ", ";

        ItemList::const_iterator itItem;
        for(itItem = items.begin(); itItem != items.end(); ++itItem)
            if((itItem->dot < itItem->rule.symbols.size()) && (itItem->rule.symbols[itItem->dot].name == *itIntermediate))
                break;

        if(itItem != items.end() && (itItem->nextState != nullptr))
            os << itItem->nextState->numState;
        else
            os << options.errorState;
    }
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::printDebugActions(std::ostream & os, const Grammar & grammar, const Options & options) const
{
    // Reduce rule
    if((items.size() >= 1) && (items.begin()->getType() == Item::Type::REDUCE) && (items.begin()->rule.numRule > 1))
    {
        for(Dictionary::const_iterator idx = grammar.terminals.begin(); idx != grammar.terminals.end(); ++idx)
            os << 'R' << std::setw(idx->length() - 1) << std::left << items.begin()->rule.numRule << '|';

        os << 'R' << std::setw(options.endOfInputToken.length() - 1) << std::left << items.begin()->rule.numRule << '|';

        return;
    }

    // Shift rule
    for(const std::string & terminal : grammar.terminals)
    {
        ItemList::const_iterator it;
        for(it = items.begin(); it != items.end(); ++it)
        {
            if((it->dot < it->rule.symbols.size()) && (it->rule.symbols[it->dot].name == terminal))
            {
                os << 'S' << std::setw(terminal.length() - 1) << std::left << (it->nextState != nullptr ? it->nextState->numState : -1) << '|';
                break;
            }
        }

        if(it == items.end())
            os << std::setw(terminal.length() + 1) << std::right << '|';
    }

    // Accept rule
    ItemList::const_iterator it;
    for(it = items.begin(); it != items.end(); ++it)
    {
        if((it->rule == grammar.getStartRule()) && (it->dot >= it->rule.symbols.size()))
        {
            os << std::setw(options.endOfInputToken.length() - 1) << std::left << "ACC" << '|';
            break;
        }
    }

    if(it == items.end())
        os << std::setw(options.endOfInputToken.length() + 1) << std::right << '|';
}

////////////////////////////////////////////////////////////////////////////////
void ParserState::printDebugBranches(std::ostream & os, const Grammar & grammar, std::size_t size) const
{
    for(const std::string & intermediate : grammar.intermediates)
    {
        if(intermediate != grammar.START_RULE)
        {
            ItemList::const_iterator it;
            for(it = items.begin(); it != items.end(); ++it)
            {
                const Symbol & symbol = it->rule.symbols[it->dot];

                if((it->getType() == Item::Type::SHIFT) && (symbol.type == Symbol::Type::INTERMEDIATE) && (symbol.name == intermediate))
                {
                    os << std::setw(std::max(intermediate.length(), size)) << std::left << (it->nextState != nullptr ? it->nextState->numState : -1) << '|';
                    break;
                }
            }

            if(it == items.end())
                os << std::setw(std::max(intermediate.length(), size) + 1) << std::right << '|';
        }
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
void ParserState::generateActionItems(std::ostream & os, Options & options, const Grammar & grammar) const
{
    // Reduce
    if((items.size() >= 1) && (items.begin()->getType() == Item::Type::REDUCE) && (items.begin()->rule.numRule > 1))
    {
        const Item & item = *items.begin();

        os << options.indent << '{' << std::endl;
        options.indent++;

        // Returned value
        os << options.indent
                << Options::VAR_RETURN << " = "
                << checkedStringReplace(options.getValue, Options::VAR_VALUE_IDX, std::to_string(item.rule.symbols.size() - 1))
                << ';' << std::endl;

        // Rule action code
        if(!item.rule.action.empty())
        {
            if(item.rule.action.find_first_of("\n\r") != std::string::npos)
                os << std::endl << item.rule.action << std::endl << std::endl;
            else
                os << std::endl << options.indent << item.rule.action << std::endl << std::endl;
        }

        // Values stack
        os << options.indent << checkedStringReplace(options.popValues, Options::VAR_NB_VALUES, std::to_string(item.rule.symbols.size()))  << std::endl;
        os << options.indent << checkedStringReplace(options.pushValue, Options::VAR_VALUE,     Options::VAR_RETURN)                       << std::endl;

        // States stack
        os << options.indent << checkedStringReplace(options.popState, Options::VAR_NB_STATES, std::to_string(item.rule.symbols.size())) << std::endl;

        // New state
        if(options.useTableForBranches)
            os << options.indent << "return " << options.branchFunctionName << "[(" << grammar.intermediates.size() << "*" << options.topState << ") + " << grammar.intermediates.index(item.rule.name) << "];" << std::endl;
        else
            os << options.indent << "return " << options.branchFunctionName << "(" << grammar.intermediates.index(item.rule.name) << ");" << std::endl;

        os << options.indent << "break;" << std::endl;

        options.indent--;
        os << options.indent << '}' << std::endl;

        return;
    }

    os << options.indent << "switch(" << checkedStringReplace(options.getTypeOfToken, Options::VAR_TOKEN, options.tokenName) << ")" << std::endl;
    os << options.indent << "{" << std::endl;

    options.indent++;
    for(const Item & item : items)
    {
        // Accept
        if((item.rule == grammar.getStartRule()) && (item.dot >= item.rule.symbols.size()))
        {
            os << options.indent << "case " << options.tokenPrefix << options.endOfInputToken << " : return " << options.acceptState << "; break;" << std::endl;
            continue;
        }

        // Shift
        if((item.dot < item.rule.symbols.size()) && (item.rule.symbols[item.dot].type == Symbol::Type::TERMINAL))
        {
            os << options.indent << "case " << options.tokenPrefix << item.rule.symbols[item.dot] << " :";
            os << ' ' << Options::VAR_RETURN << '.' << options.tokenUnionName << " = " << options.tokenName << ';';
            os << ' ' << options.shiftToken;
            os << ' ' << checkedStringReplace(options.pushValue, Options::VAR_VALUE, Options::VAR_RETURN);
            if(item.nextState != nullptr)
                os << " return " << item.nextState->numState << ';';
            else
                os << " return " << options.errorState << ';';
            os << " break;" << std::endl;
            continue;
        }
    }
    options.indent--;

    if(options.defaultSwitchStatement)
    {
        options.indent++;
        os << options.indent << "default : return " << options.errorState << "; break;" << std::endl;
        options.indent--;
    }

    os << options.indent << "}" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
std::string ParserState::checkedStringReplace(const std::string & str, const std::string & pattern, const std::string & replacement) const
{
    // Check that replacement string doesn't contains the pattern to avoid infinite loop
    if(replacement.find(pattern) != std::string::npos)
        return str;

    // Replace each occurrence
    std::string replacedStr(str);
    std::size_t pos = 0;

    while((pos = replacedStr.find(pattern, pos)) != std::string::npos)
        replacedStr.replace(pos, pattern.size(), replacement);

    return replacedStr;
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
