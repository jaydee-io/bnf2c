////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef GRAMMAR_H
#define GRAMMAR_H
#include "Rule.h"
#include "Symbol.h"
#include "Errors.h"

#include <unordered_map>
#include <unordered_set>
#include <string>

struct Options;

class Grammar
{
    public :
        typedef std::unordered_set<std::string> Dictionary;
        typedef std::unordered_map<std::string, std::string>      IntermediateTypeDictionary;

        typedef std::unordered_multimap<std::string, Rule>  RuleMap;
        typedef RuleMap::const_iterator                     RuleIterator;
        typedef std::pair<RuleIterator, RuleIterator>       RuleRange;

        static const std::string START_RULE;

    public :
        void         addRule(Rule & rule);
        const Rule & getStartRule(void) const;
        RuleRange    operator[](const std::string & name) const;

        Symbol addTerminal(const std::string & name);
        Symbol addTerminal(std::string && name);
        Symbol addIntermediate(const std::string & name);
        Symbol addIntermediate(std::string && name);

        const std::string & getIntermediateType(const std::string & name) const;
        size_t getIntermediateIndex(const std::string & name) const;

        SymbolSet first(const SymbolList & list) const;

        void replacePseudoVariables(Options & options);
        void check(void);

        Errors<GeneratingError> errors;

    public :
        RuleMap                     rules;

        Dictionary                  terminals;
        Dictionary                  intermediates;

        IntermediateTypeDictionary intermediateTypes;
};

#endif /* GRAMMAR_H */
