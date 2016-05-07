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
Options     Options::DEFAULT;
const std::string Options::VAR_NB_STATES      ("<NB_STATES>");
const std::string Options::VAR_VALUE          ("<VALUE>");
const std::string Options::VAR_VALUE_IDX      ("<VALUE_IDX>");
const std::string Options::VAR_NB_VALUES      ("<NB_VALUES>");
const std::string Options::VAR_EXTERNAL_RETURN("$$");
const std::string Options::VAR_RETURN         ("yylval");
const std::string Options::VAR_TOKEN          ("<TOKEN>");
const std::string Options::VAR_TYPE           ("<TYPE>");
const std::string Options::VERSION            ("0.3");

const struct option Options::OPTIONS [] = {
    { "help",                   no_argument,       nullptr, 'h'},
    { "version",                no_argument,       nullptr, 'v'},
    { "debug",                  required_argument, nullptr, 'd'},

    // Parser options
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
    { "value-as-intermediate",  required_argument, nullptr, 'm'},

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
#define NB_OPTIONS_PARSER    11
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
    for(int i=0; i<(ARRAY_SIZE(Options::OPTIONS) - 1); i++)
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
    for(int i=0, j=0; i<(ARRAY_SIZE(Options::OPTIONS) - 1); i++)
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
            case 's' : stateType.assign(optarg);           break;
            case 't' : topState.assign(optarg);            break;
            case 'p' : popState.assign(optarg);            break;
            case 'e' : errorState.assign(optarg);          break;
            case 'a' : acceptState.assign(optarg);         break;

            case 'q' : valueType.assign(optarg);           break;
            case 'g' : pushValue.assign(optarg);           break;
            case 'j' : popValues.assign(optarg);           break;
            case 'k' : getValue.assign(optarg);            break;
            case 'm' : valueAsToken.assign(optarg);        break;
            case 'z' : valueAsIntermediate.assign(optarg); break;

            case 'y' : tokenType.assign(optarg);           break;
            case 'c' : shiftToken.assign(optarg);          break;
            case 'r' : tokenPrefix.assign(optarg);         break;
            case 'l' : getTypeOfToken.assign(optarg);      break;
            case 'f' : endOfInputToken.assign(optarg);     break;

            case 'i' : intermediateType.assign(optarg);    break;
            case 'n' : parseFunctionName.assign(optarg);   break;
            case 'b' : branchFunctionName.assign(optarg);  break;
            case 'x' : throwedExceptions.assign(optarg);   break;

            case 'w' : defaultSwitchStatement = true;      break;
            case 'u' : useTableForBranches    = true;      break;

            case 'o' : m_outputFileName.assign(optarg);    break;

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
                std::cout << "Copyright © 2013, Jérôme DUMESNIL" << std::endl;
                std::cout << "License BSD" << std::endl;
                std::cout << "https://code.google.com/p/bnf2c" << std::endl << std::endl;
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
        m_inputFileName.assign(argv[optind]);
}

////////////////////////////////////////////////////////////////////////////////
#define USAGE_OPTION(nbOptions) \
    do { \
        for(int max=i + nbOptions; i<max; i++) \
        { \
            std::cout << "  -" << (char) Options::OPTIONS[i].val << ", --" << std::left << std::setw(22) << Options::OPTIONS[i].name  << "    " << Options::OPTIONS_TEXT[i][0] << std::endl; \
            for(int j=1; j<Options::OPTIONS_TEXT[i].size(); j++) \
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
std::istream & Options::inputStream(void)
{
    if(m_inputFileName.empty())
        return std::cin;
    else
    {
        m_inputFileStream.open(m_inputFileName);

        if(m_inputFileStream.fail())
            ADD_COMMAND_LINE_PARSING_ERROR(1, "Unable to open input file \"" << m_inputFileName << "\"");

        return m_inputFileStream;
    }
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & Options::outputStream(void)
{
    if(m_outputFileName.empty())
        return std::cout;
    else
    {
        m_outputFileStream.open(m_outputFileName);

        if(m_outputFileStream.fail())
            ADD_COMMAND_LINE_PARSING_ERROR(1, "Unable to open output file \"" << m_outputFileName << "\"");

        return m_outputFileStream;
    }
}

////////////////////////////////////////////////////////////////////////////////
#define SET_OPTION_IF_NOT_DEFAULT(option) if(options.option != Options::DEFAULT.option) option = options.option
Options & Options::operator <<(const Options & options)
{
    // Parser options
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
    SET_OPTION_IF_NOT_DEFAULT(m_inputFileName);
    SET_OPTION_IF_NOT_DEFAULT(m_outputFileName);

    return (*this);
}

////////////////////////////////////////////////////////////////////////////////
#define DISPLAY_OPTION(option) os << #option << " = \"" << options.option << '"' << std::endl
std::ostream & operator <<(std::ostream & os, const Options & options)
{
    // Parser options
    DISPLAY_OPTION(stateType          );
    DISPLAY_OPTION(topState           );
    DISPLAY_OPTION(popState           );
    DISPLAY_OPTION(errorState         );
    DISPLAY_OPTION(acceptState        );
    DISPLAY_OPTION(valueType          );
    DISPLAY_OPTION(pushValue          );
    DISPLAY_OPTION(popValues          );
    DISPLAY_OPTION(getValue           );
    DISPLAY_OPTION(valueAsToken       );
    DISPLAY_OPTION(valueAsIntermediate);

    // Lexer options
    DISPLAY_OPTION(tokenType      );
    DISPLAY_OPTION(shiftToken     );
    DISPLAY_OPTION(tokenPrefix    );
    DISPLAY_OPTION(getTypeOfToken );
    DISPLAY_OPTION(endOfInputToken);

    // Generated code options
    DISPLAY_OPTION(intermediateType  );
    DISPLAY_OPTION(parseFunctionName );
    DISPLAY_OPTION(branchFunctionName);

    DISPLAY_OPTION(defaultSwitchStatement);
    DISPLAY_OPTION(useTableForBranches   );

    DISPLAY_OPTION(tokenName       );
    DISPLAY_OPTION(intermediateName);

    return os;
}
