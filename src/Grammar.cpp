////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "Grammar.h"

#include <iostream>
#include <regex>


#define ADD_GENERATING_ERROR(message)\
    do {\
        std::stringstream ss;\
        ss << message;\
        errors.list.push_back(GeneratingError({ss.str()}));\
    } while(false)


////////////////////////////////////////////////////////////////////////////////
const std::string Grammar::START_RULE("START");

////////////////////////////////////////////////////////////////////////////////
void Grammar::addRule(Rule & rule)
{
    rule.numRule = rules.size() + 1;
    rules.insert(RuleMap::value_type(rule.name, rule));
}

const Rule & Grammar::getStartRule(void) const
{
    RuleIterator it = rules.find(Grammar::START_RULE);

    return it->second;
}

////////////////////////////////////////////////////////////////////////////////
Grammar::RuleRange Grammar::operator[](const std::string & name) const
{
    return rules.equal_range(name);
}

////////////////////////////////////////////////////////////////////////////////
Symbol Grammar::addTerminal(const std::string & name)
{
    terminals.add(name);
    return Symbol({Symbol::Type::TERMINAL, name});
}

////////////////////////////////////////////////////////////////////////////////
Symbol Grammar::addTerminal(std::string && name)
{
    terminals.add(name);
    return Symbol({Symbol::Type::TERMINAL, name});
}

////////////////////////////////////////////////////////////////////////////////
Symbol Grammar::addIntermediate(const std::string & name)
{
    intermediates.add(name);
    return Symbol({Symbol::Type::INTERMEDIATE, name});
}

////////////////////////////////////////////////////////////////////////////////
Symbol Grammar::addIntermediate(std::string && name)
{
    intermediates.add(name);
    return Symbol({Symbol::Type::INTERMEDIATE, name});
}

////////////////////////////////////////////////////////////////////////////////
const std::string & Grammar::getIntermediateType(const std::string & name) const
{
    return intermediateTypes.at(name);
}

////////////////////////////////////////////////////////////////////////////////
void Grammar::replacePseudoVariables(Options & options)
{
    for(RuleMap::value_type & pair : rules)
    {
        Rule & rule = pair.second;
        if(!rule.action.empty())
        {
            std::string replacement;

            // Replace return pseudo-variable '$$'
            replacement = checkedStringReplace(options.valueAsIntermediate, Options::VAR_VALUE, Options::VAR_RETURN);
            replacement = checkedStringReplace(replacement, Options::VAR_TYPE, intermediateTypes.at(rule.name));
            rule.action = checkedStringReplace(rule.action, Options::VAR_EXTERNAL_RETURN, replacement);

            // Replace pseudo-variables '$1', '$2', ... '$n'
            for(int i = 1; i <= rule.symbols.size(); i++)
            {
                if(rule.symbols[i-1].type == Symbol::Type::INTERMEDIATE)
                {
                    replacement = checkedStringReplace(options.valueAsIntermediate, Options::VAR_VALUE, options.getValue);
                    replacement = checkedStringReplace(replacement, Options::VAR_TYPE, intermediateTypes.at(rule.symbols[i-1].name));
                }
                else
                {
                    replacement = checkedStringReplace(options.valueAsToken, Options::VAR_VALUE, options.getValue);
                    replacement = checkedStringReplace(replacement, Options::VAR_TYPE, options.tokenType);
                }

                replacement = checkedStringReplace(replacement, Options::VAR_VALUE_IDX, std::to_string(rule.symbols.size() - i));

                rule.action = checkedStringReplace(rule.action, '$' + std::to_string(i), replacement);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void Grammar::check(void)
{
    // Check start rule
    if(intermediates.contains(Grammar::START_RULE))
    {
        Grammar::RuleMap::size_type nbStartsRules = rules.count(Grammar::START_RULE);

        if(nbStartsRules == 0)
            ADD_GENERATING_ERROR("No start rule '" + Grammar::START_RULE + "' found");
        else if(nbStartsRules > 1)
            ADD_GENERATING_ERROR("Multiple start rules \"" + Grammar::START_RULE + "\" found");
    }
    else
        ADD_GENERATING_ERROR("No start rule '" + Grammar::START_RULE + "' found");

    // Check intermediates types
    for(Dictionary::const_iterator intermediate = intermediates.begin(); intermediate != intermediates.end(); ++intermediate)
        if(intermediateTypes.find(*intermediate) == intermediateTypes.end())
            ADD_GENERATING_ERROR("Intermediate '" + (*intermediate) + "' has no type");
}

////////////////////////////////////////////////////////////////////////////////
std::string Grammar::checkedStringReplace(const std::string & str, const std::string & pattern, const std::string & replacement) const
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
std::ostream & operator <<(std::ostream & os, const Grammar & grammar)
{
    os << "Grammar contains " << grammar.rules.size() << " rules" << std::endl;
    for(Grammar::RuleMap::const_iterator it = grammar.rules.begin(); it != grammar.rules.end(); ++it)
        os << it->second << std::endl;

    return os;
}
