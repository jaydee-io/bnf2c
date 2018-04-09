////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "Options.h"

#include <sstream>
#include <iostream>
#include <iomanip>

////////////////////////////////////////////////////////////////////////////////
Indenter & Indenter::operator ++(int)
{
    indent++;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
Indenter & Indenter::operator --(int)
{
    indent--;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
Indenter & Indenter::operator ++(void)
{
    indent++;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
Indenter & Indenter::operator --(void)
{
    indent--;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
bool Indenter::operator !=(const Indenter & indenter) const
{
    return (string != indenter.string) || (top != indenter.top) || (indent != indenter.indent);
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Indenter & indenter)
{
    for(unsigned int i = 0; i < (indenter.top + indenter.indent); i++)
        os << indenter.string;

    return os;
}


#define ARRAY_SIZE(x) (sizeof(x)/sizeof(0[x]))

#define ADD_COMMAND_LINE_PARSING_ERROR(code, message)\
    do {\
        std::stringstream ss;\
        ss << message;\
        errors.list.push_back(CommandLineParsingError{ss.str()});\
        errors.exitCode = code;\
    } while(false)

////////////////////////////////////////////////////////////////////////////////
Options Options::DEFAULT;
const std::string Options::VERSION            ("0.5");

const struct option Options::OPTIONS [] = {
    { "help",                   no_argument,       nullptr, 'h'},
    { "version",                no_argument,       nullptr, 'v'},
    { "debug",                  required_argument, nullptr, 'd'},

    // Parser options
    { "parser-type",            required_argument, nullptr, 'T'},
    { "state-type",             required_argument, nullptr, 's'},
    { "top-state-code",         required_argument, nullptr, 't'},
    { "pop-state-code",         required_argument, nullptr, 'p'},
    { "error-state",            required_argument, nullptr, 'e'},
    { "accept-state",           required_argument, nullptr, 'a'},

    { "value-type",             required_argument, nullptr, 'q'},
    { "push-value-code",        required_argument, nullptr, 'g'},
    { "pop-values-code",        required_argument, nullptr, 'j'},
    { "get-value-code",         required_argument, nullptr, 'k'},
    { "value-as-token",         required_argument, nullptr, 'm'},
    { "value-as-intermediate",  required_argument, nullptr, 'z'},
    { "default-action",         required_argument, nullptr, 'D'},

    // Lexer options
    { "token-type",             required_argument, nullptr, 'y'},
    { "shift-token-code",       required_argument, nullptr, 'c'},
    { "token-prefix",           required_argument, nullptr, 'r'},
    { "get-type-of-token-code", required_argument, nullptr, 'l'},
    { "end-of-input-token",     required_argument, nullptr, 'f'},

    // Generated code options
    { "intermediate-type",      required_argument, nullptr, 'i'},
    { "parse-function",         required_argument, nullptr, 'n'},
    { "branch-function",        required_argument, nullptr, 'b'},
    { "throwed-exceptions",     required_argument, nullptr, 'x'},

    { "default-switch",         no_argument,       nullptr, 'w'},
    { "use-table-for-branches", no_argument,       nullptr, 'u'},
    { "output",                 required_argument, nullptr, 'o'},
    { nullptr,                  no_argument,       nullptr,  0}
};

const std::vector<std::string> Options::OPTIONS_TEXT [] = {
        { "Print this help" },
        { "Print version number" },
        { "Print addtionnal debugging information on stderr",
          "  - 1 : Debug generator",
          "  - 2 : Debug parser",
          "  - 3 : Debug lexer" },

        { "Type of generated parser : LR0, LR1 or LALR1" },
        { "Type used for generated states" },
        { "Code used to get the state on top of the stack" },
        { "Code used to pop states from the stack" },
        { "State number used to specify an error" },
        { "State number used when parsing is done and the input is accepted" },
        { "Type regrouping intermediate and token values" },
        { "Code used to push a new value on the stack" },
        { "Code used to pop values from the stack" },
        { "Code used to get the N-th value from the stack" },
        { "Code used to get a value as token" },
        { "Code used to get a value as intermediate" },
        { "Default action to execute when no action specified" },

        { "Type used for tokens" },
        { "Code used to move lexer to the next token" },
        { "Prefix of token" },
        { "Code used to get the type of token" },
        { "Name of the token use to specify end of input stream" },

        { "Type used for generated intermediate type" },
        { "Name of the generated parse function" },
        { "Name of the generated branch function" },
        { "Names of the exceptions throwed by generated functions (default no exceptions throwed)" },
        { "Generate a default statement in switch / case (default no default case)" },
        { "Use table instead of a function for branches (default use function)" },

        { "Specify the name of the output file (default to stdout)" }
};

#define NB_OPTIONS_COMMON    3
#define NB_OPTIONS_PARSER    13
#define NB_OPTIONS_LEXER     5
#define NB_OPTIONS_GENERATOR 6
#define NB_OPTIONS_FILE      1

////////////////////////////////////////////////////////////////////////////////
void Options::parseArguments(int argc, char ** argv)
{
    int optionIndex = 0;
    int option = 0;

    // Construct short options table
    int nbShortOptions = 0;
    for(size_t i=0; i<(ARRAY_SIZE(Options::OPTIONS) - 1); i++)
    {
        switch(Options::OPTIONS[i].has_arg)
        {
            case no_argument       : nbShortOptions += 1; break;
            case required_argument : nbShortOptions += 2; break;
            case optional_argument : nbShortOptions += 3; break;
            default                : /* do nothing */;    break;
        }
    }
    char * shortOptions = new char[nbShortOptions];
    for(size_t i=0, j=0; i<(ARRAY_SIZE(Options::OPTIONS) - 1); i++)
    {
        shortOptions[j++] = Options::OPTIONS[i].val;
        switch(Options::OPTIONS[i].has_arg)
        {
            case optional_argument : shortOptions[j++] = ':'; /* no break */
            case required_argument : shortOptions[j++] = ':'; break;
            default                : /* do nothing */;        break;
        }
    }

    // Parse options
    opterr = 0;
    while ((option = getopt_long(argc, argv, shortOptions, Options::OPTIONS, &optionIndex)) != -1)
    {
        switch (option)
        {
            case 'T' : parserType.assign(optarg);          break;
            case 's' : stateType.assign(optarg);           break;
            case 't' : topState.assign(optarg);            break;
            case 'p' : popState = optarg;                  break;
            case 'e' : errorState.assign(optarg);          break;
            case 'a' : acceptState.assign(optarg);         break;

            case 'q' : valueType.assign(optarg);           break;
            case 'g' : pushValue = optarg;                 break;
            case 'j' : popValues = optarg;                 break;
            case 'k' : getValue = optarg;                  break;
            case 'm' : valueAsToken = optarg;              break;
            case 'z' : valueAsIntermediate = optarg;       break;
            case 'D' : defaultAction = optarg;             break;

            case 'y' : tokenType.assign(optarg);           break;
            case 'c' : shiftToken.assign(optarg);          break;
            case 'r' : tokenPrefix.assign(optarg);         break;
            case 'l' : getTypeOfToken = optarg;            break;
            case 'f' : endOfInputToken.assign(optarg);     break;

            case 'i' : intermediateType.assign(optarg);    break;
            case 'n' : parseFunctionName.assign(optarg);   break;
            case 'b' : branchFunctionName.assign(optarg);  break;
            case 'x' : throwedExceptions.assign(optarg);   break;

            case 'w' : defaultSwitchStatement = true;      break;
            case 'u' : useTableForBranches    = true;      break;

            case 'o' : outputFileName.assign(optarg);    break;

            case 'd' :
            {
                int debugLevelInt = 0;
                std::istringstream iss(optarg);
                iss >> debugLevelInt;
                debugLevel = (DebugLevel) debugLevelInt;
                break;
            }
            case 'v' :
                std::cout << "bnf2c version " << Options::VERSION << std::endl;
                std::cout << "Copyright © 2013 - 2016, Jérôme DUMESNIL" << std::endl;
                std::cout << "BSD License" << std::endl;
                std::cout << "https://github.com/jaydee-io/bnf2c" << std::endl << std::endl;
                std::cout << "Written by Jérôme DUMESNIL" << std::endl;
                ::exit(0);
                break;

            case 'h' :
                ADD_COMMAND_LINE_PARSING_ERROR(0, "");
                break;

            case ':' :
            {
                ADD_COMMAND_LINE_PARSING_ERROR(1, "Missing argument for option '-" << (char) optopt << "'");
                break;
            }
            case '?' :
            default :
            {
                ADD_COMMAND_LINE_PARSING_ERROR(1, "Unknown option '-" << (char) optopt << "'");
                break;
            }
        }
    }

    delete [] shortOptions;

    if (optind < argc)
        inputFileName.assign(argv[optind]);
}

////////////////////////////////////////////////////////////////////////////////
#define USAGE_OPTION(nbOptions) \
    do { \
        for(int max=i + nbOptions; i<max; i++) \
        { \
            std::cout << "  -" << (char) Options::OPTIONS[i].val << ", --" << std::left << std::setw(22) << Options::OPTIONS[i].name  << "    " << Options::OPTIONS_TEXT[i][0] << std::endl; \
            for(size_t j=1; j<Options::OPTIONS_TEXT[i].size(); j++) \
                std::cout << std::left << std::setw(34) << ' ' << Options::OPTIONS_TEXT[i][j] << std::endl; \
        } \
    } while(false)

void Options::usage(void)
{
    Options options;
    int i = 0;

    std::cout << "Usage : bnf2c [OPTIONS] [InputBnfFile]" << std::endl;

    std::cout << std::endl;
    USAGE_OPTION(NB_OPTIONS_COMMON);

    std::cout << std::endl << "Parser options :" << std::endl;
    USAGE_OPTION(NB_OPTIONS_PARSER);

    std::cout << std::endl << "Lexer options :" << std::endl;
    USAGE_OPTION(NB_OPTIONS_LEXER);

    std::cout << std::endl << "Generated code options :" << std::endl;
    USAGE_OPTION(NB_OPTIONS_GENERATOR);

    std::cout << std::endl << "File :" << std::endl;
    USAGE_OPTION(NB_OPTIONS_FILE);
}

////////////////////////////////////////////////////////////////////////////////
#define SET_OPTION_IF_NOT_DEFAULT(option) if(options.option != Options::DEFAULT.option) option = options.option
Options & Options::operator <<(const Options & options)
{
    // Parser options
    SET_OPTION_IF_NOT_DEFAULT(parserType);
    SET_OPTION_IF_NOT_DEFAULT(stateType);
    SET_OPTION_IF_NOT_DEFAULT(topState);
    SET_OPTION_IF_NOT_DEFAULT(popState);
    SET_OPTION_IF_NOT_DEFAULT(errorState);
    SET_OPTION_IF_NOT_DEFAULT(acceptState);

    SET_OPTION_IF_NOT_DEFAULT(valueType);
    SET_OPTION_IF_NOT_DEFAULT(pushValue);
    SET_OPTION_IF_NOT_DEFAULT(popValues);
    SET_OPTION_IF_NOT_DEFAULT(getValue);
    SET_OPTION_IF_NOT_DEFAULT(valueAsToken);
    SET_OPTION_IF_NOT_DEFAULT(valueAsIntermediate);
    SET_OPTION_IF_NOT_DEFAULT(defaultAction);

    // Lexer options
    SET_OPTION_IF_NOT_DEFAULT(tokenType);
    SET_OPTION_IF_NOT_DEFAULT(shiftToken);
    SET_OPTION_IF_NOT_DEFAULT(tokenPrefix);
    SET_OPTION_IF_NOT_DEFAULT(getTypeOfToken);
    SET_OPTION_IF_NOT_DEFAULT(endOfInputToken);

    // Generated code options
    SET_OPTION_IF_NOT_DEFAULT(intermediateType);
    SET_OPTION_IF_NOT_DEFAULT(parseFunctionName);
    SET_OPTION_IF_NOT_DEFAULT(branchFunctionName);
    SET_OPTION_IF_NOT_DEFAULT(throwedExceptions);
    SET_OPTION_IF_NOT_DEFAULT(defaultSwitchStatement);
    SET_OPTION_IF_NOT_DEFAULT(useTableForBranches);
    SET_OPTION_IF_NOT_DEFAULT(tokenName);
    SET_OPTION_IF_NOT_DEFAULT(intermediateName);
    SET_OPTION_IF_NOT_DEFAULT(debugLevel);

    SET_OPTION_IF_NOT_DEFAULT(indent);
    SET_OPTION_IF_NOT_DEFAULT(inputFileName);
    SET_OPTION_IF_NOT_DEFAULT(outputFileName);

    return (*this);
}

