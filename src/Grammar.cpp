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

#include <iostream>
#include <regex>

const std::string Grammar::START_RULE("START");

////////////////////////////////////////////////////////////////////////////////
void Grammar::addRule(Rule & rule)
{
    rule.numRule = rules.size() + 1;
    rules.insert(RuleMap::value_type(rule.name, rule));
}

const Rule & Grammar::getStartRule(void) const
{
    RuleIterator it = rules.find(intermediates[Grammar::START_RULE]);

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
void Grammar::replacePseudoVariables(Options & options)
{
    for(RuleMap::value_type & pair : rules)
    {
        Rule & rule = pair.second;
        if(!rule.action.empty())
        {
            std::size_t pos = 0;

            // Replace return pseudo-variable '$$'
            while((pos = rule.action.find(Options::VAR_EXTERNAL_RETURN, pos)) != std::string::npos)
                rule.action.replace(pos, Options::VAR_EXTERNAL_RETURN.size(), Options::VAR_RETURN + '.' + intermediateTypes.at(intermediates[rule.name]));

            // Replace pseudo-variables '$1', '$2', ... '$n'
            // For now (21 september 2013), regexp is not implemented in libstdc++,
            // so manually look for pseudo-variables 1 to 100
            for(int i = 1; i <= 100; i++)
            {
                pos = 0;
                std::string replacement(options.getValue);
                while((pos = replacement.find(Options::VAR_VALUE_IDX, pos)) != std::string::npos)
                    replacement.replace(pos, Options::VAR_VALUE_IDX.size(), std::to_string(rule.symbols.size() - i));

                if(i <= rule.symbols.size())
                    replacement += "." + ((rule.symbols[i-1].type == Symbol::Type::INTERMEDIATE) ? intermediateTypes.at(*rule.symbols[i-1].name) : options.tokenUnionName);

                pos = 0;
                std::string pseudoVar('$' + std::to_string(i));
                while((pos = rule.action.find(pseudoVar, pos)) != std::string::npos)
                    rule.action.replace(pos, pseudoVar.size(), replacement);
            }

            /*
            std::string replacement(options.getValue);
            pos = replacement.find(Options::VAR_VALUE_IDX, pos);
            if(std::string::npos != pos)
                replacement.replace(0, Options::VAR_VALUE_IDX.size(), std::to_string(rule.symbols.size()) + " - \\1");

            try
            {
                std::regex regexp("\\$([:digit:]+)");
                std::regex_replace(rule.action, regexp, replacement);
            }
            catch (std::regex_error & e)
            {
                // TODO We should throw an exception...
            }
            */
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Grammar & grammar)
{
    os << "Grammar contains " << grammar.rules.size() << " rules" << std::endl;
    for(Grammar::RuleMap::const_iterator it = grammar.rules.begin(); it != grammar.rules.end(); ++it)
        os << it->second << std::endl;

    return os;
}
