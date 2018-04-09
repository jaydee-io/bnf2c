////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "Grammar.h"
#include "config/Options.h"
#include "printer/PrettyPrinters.h"

#include <sstream>


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
    terminals.insert(name);
    return Symbol({Symbol::Type::TERMINAL, name});
}

////////////////////////////////////////////////////////////////////////////////
Symbol Grammar::addTerminal(std::string && name)
{
    terminals.insert(name);
    return Symbol({Symbol::Type::TERMINAL, name});
}

////////////////////////////////////////////////////////////////////////////////
Symbol Grammar::addIntermediate(const std::string & name)
{
    intermediates.insert(name);
    return Symbol({ Symbol::Type::INTERMEDIATE, name });
}

////////////////////////////////////////////////////////////////////////////////
Symbol Grammar::addIntermediate(std::string && name)
{
    intermediates.insert(name);
    return Symbol({ Symbol::Type::INTERMEDIATE, std::forward<std::string>(name) });
}

////////////////////////////////////////////////////////////////////////////////
const std::string & Grammar::getIntermediateType(const std::string & name) const
{
    return intermediateTypes.at(name);
}

////////////////////////////////////////////////////////////////////////////////
size_t Grammar::getIntermediateIndex(const std::string & name) const
{
    return std::distance(intermediates.begin(), intermediates.find(name));
}

////////////////////////////////////////////////////////////////////////////////
SymbolSet Grammar::first(const SymbolList & list) const
{
    if(list[0].isTerminal())
        return { list[0] };

    SymbolSet firstSet;
    const auto intermediateRules = rules.equal_range(list[0].name);
    for(auto itRule = intermediateRules.first; itRule != intermediateRules.second; ++itRule)
    {
        if(itRule->second.symbols != list)
        {
            const auto firstIntermediate = first(itRule->second.symbols);
            firstSet.insert(firstIntermediate.begin(), firstIntermediate.end());
        }
    }

    return firstSet;
}

////////////////////////////////////////////////////////////////////////////////
void Grammar::replacePseudoVariables(Options & options)
{
    for(RuleMap::value_type & pair : rules)
    {
        Rule & rule = pair.second;
        if(rule.action.empty())
            rule.action = options.defaultAction.toString();

        // Replace return pseudo-variable '$$'
        ParameterizedString replacement = options.valueAsIntermediate
            .replaceParam(Vars::VALUE, Vars::RETURN)
            .replaceParam(Vars::TYPE, intermediateTypes.at(rule.name));
        ParameterizedString parameterizedAction = ParameterizedString(rule.action)
            .replaceParam(Vars::EXTERNAL_RETURN, replacement.toString());

        // Replace pseudo-variables '$1', '$2', ... '$n'
        for(size_t i = 1; i <= rule.symbols.size(); i++)
        {
            if(rule.symbols[i-1].isIntermediate())
            {
                replacement = options.valueAsIntermediate
                    .replaceParam(Vars::VALUE, options.getValue.toString())
                    .replaceParam(Vars::TYPE, intermediateTypes.at(rule.symbols[i-1].name))
                    .replaceParam(Vars::VALUE_IDX, std::to_string(rule.symbols.size() - i));
            }
            else
            {
                replacement = options.valueAsToken
                    .replaceParam(Vars::VALUE, options.getValue.toString())
                    .replaceParam(Vars::TYPE,  options.tokenType)
                    .replaceParam(Vars::VALUE_IDX, std::to_string(rule.symbols.size() - i));
            }

            parameterizedAction = parameterizedAction.replaceParam('$' + std::to_string(i), replacement.toString());
        }

        rule.action = parameterizedAction.toString();
    }
}

////////////////////////////////////////////////////////////////////////////////
void Grammar::check(void)
{
    // Check start rule
    if(intermediates.find(Grammar::START_RULE) != intermediates.end())
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
