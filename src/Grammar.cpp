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
#include "Grammar.h"

#include <stdexcept>
#include <iostream>

const std::string Grammar::START_RULE("START");

////////////////////////////////////////////////////////////////////////////////
void Grammar::addRule(const Rule & rule)
{
    rules.insert(RuleMap::value_type(rule.name, rule));
}

const Rule & Grammar::getStartRule(void) const
{
    RuleIterator it = rules.find(intermediates[Grammar::START_RULE]);

    if(it == rules.end())
        throw std::runtime_error("No rule named \"" + Grammar::START_RULE + "\" found !!!");

    return it->second;
}

////////////////////////////////////////////////////////////////////////////////
Grammar::RuleRange Grammar::operator[](const Dictionnary::Index & name) const
{
    return rules.equal_range(name);
}

////////////////////////////////////////////////////////////////////////////////
Symbol Grammar::addTerminal(const std::string & name)
{
    Dictionnary::Index index = terminals.add(name);
    return Symbol({Symbol::Type::TERMINAL, index});
}

////////////////////////////////////////////////////////////////////////////////
Symbol Grammar::addTerminal(std::string && name)
{
    Dictionnary::Index index = terminals.add(name);
    return Symbol({Symbol::Type::TERMINAL, index});
}

////////////////////////////////////////////////////////////////////////////////
Symbol Grammar::addIntermediate(const std::string & name)
{
    Dictionnary::Index index = intermediates.add(name);
    return Symbol({Symbol::Type::INTERMEDIATE, index});
}

////////////////////////////////////////////////////////////////////////////////
Symbol Grammar::addIntermediate(std::string && name)
{
    Dictionnary::Index index = intermediates.add(name);
    return Symbol({Symbol::Type::INTERMEDIATE, index});
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Grammar & grammar)
{
    os << "Grammar contains " << grammar.rules.size() << " rules" << std::endl;
    for(Grammar::RuleMap::const_iterator it = grammar.rules.begin(); it != grammar.rules.end(); ++it)
        os << it->second << std::endl;

    return os;
}
