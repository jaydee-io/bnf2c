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
#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__
#include "Rule.h"
#include "Symbol.h"
#include "Dictionnary.h"
#include "Options.h"
#include "Errors.h"

#include <unordered_map>
#include <string>
#include <set>
#include <iostream>

class Grammar
{
    public :
        typedef std::unordered_map<std::string, std::string>      IntermediateTypeDictionnary;

        typedef std::unordered_multimap<Dictionnary::Index, Rule> RuleMap;
        typedef RuleMap::const_iterator                 RuleIterator;
        typedef std::pair<RuleIterator, RuleIterator>   RuleRange;

        static const std::string START_RULE;

    public :
        void         addRule(Rule & rule);
        const Rule & getStartRule(void) const;
        RuleRange    operator[](const Dictionnary::Index & name) const;

        Symbol addTerminal(const std::string & name);
        Symbol addTerminal(std::string && name);
        Symbol addIntermediate(const std::string & name);
        Symbol addIntermediate(std::string && name);

        void replacePseudoVariables(Options & options);
        void check(void);

        Errors<GeneratingError> errors;

    public :
        RuleMap                     rules;

        Dictionnary                 terminals;
        Dictionnary                 intermediates;

        IntermediateTypeDictionnary intermediateTypes;
};

std::ostream & operator <<(std::ostream & os, const Grammar & ruleSet);

#endif /* __GRAMMAR_H__ */
