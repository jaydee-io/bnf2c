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
std::ostream & operator <<(std::ostream & os, const Indenter & indenter)
{
    for(unsigned int i = 0; i < (indenter.top + indenter.indent); i++)
        os << indenter.string;

    return os;
}

////////////////////////////////////////////////////////////////////////////////
const std::string Options::VAR_NB_STATES("<NB_STATES>");

const struct option Options::LONG_OPTIONS [] = {
        { "help",                   no_argument,       nullptr, 'h'},
        { "top-state-code",         required_argument, nullptr, 't'},
        { "pop-state-code",         required_argument, nullptr, 'p'},
        { "error-state",            required_argument, nullptr, 'e'},
        { "accept-state",           required_argument, nullptr, 'a'},

        { "state-type",             required_argument, nullptr, 's'},
        { "token-type",             required_argument, nullptr, 'y'},
        { "intermediate-type",      required_argument, nullptr, 'i'},

        { "shift-token-code",       required_argument, nullptr, 'c'},
        { "token-prefix",           required_argument, nullptr, 'r'},
        { "end-of-input-token",     required_argument, nullptr, 'f'},

        { "parse-function",         required_argument, nullptr, 'n'},
        { "branch-function",        required_argument, nullptr, 'b'},
        { "throwed-exceptions",     required_argument, nullptr, 'w'},

        { "default-switch",         no_argument,       nullptr, 'd'},
        { "use-table-for-branches", no_argument,       nullptr, 'u'},
        { "output",                 required_argument, nullptr, 'o'},
        { nullptr,                  no_argument,       nullptr,  0}
};
const char Options::SHORT_OPTIONS [] = ":ht:p:e:a:s:y:i:c:r:f:n:b:w:duo:";

////////////////////////////////////////////////////////////////////////////////
void Options::parseArguments(int argc, char ** argv) throw(CommandLineParsingError)
{
    int optionIndex = 0;
    int option = 0;

    while ((option = getopt_long(argc, argv, Options::SHORT_OPTIONS, Options::LONG_OPTIONS, &optionIndex)) != -1)
    {
        switch (option)
        {
            case 't' : topState.assign(optarg);           break;
            case 'p' : popState.assign(optarg);           break;
            case 'e' : errorState.assign(optarg);         break;
            case 'a' : acceptState.assign(optarg);        break;

            case 's' : stateType.assign(optarg);          break;
            case 'y' : tokenType.assign(optarg);          break;
            case 'i' : intermediateType.assign(optarg);   break;

            case 'c' : shiftToken.assign(optarg);         break;
            case 'r' : tokenPrefix.assign(optarg);        break;
            case 'f' : endOfInputToken.assign(optarg);    break;

            case 'n' : parseFunctionName.assign(optarg);  break;
            case 'b' : branchFunctionName.assign(optarg); break;
            case 'w' : throwedExceptions.assign(optarg);  break;

            case 'd' : defaultSwitchStatement = true;     break;
            case 'u' : useTableForBranches    = true;     break;

            case 'o' : m_outputFileName.assign(optarg);     break;

            case 'h' :
                throw CommandLineParsingError({"", 0});
                break;

            case ':' :
            {
                std::stringstream error;
                error << "Missing argument for option '-" << (char) optopt << "'";
                throw CommandLineParsingError({error.str(), 1});
                break;
            }
            case '?' :
            default :
            {
                std::stringstream error;
                error << "Unknown option '-" << (char) optopt << "'";
                throw CommandLineParsingError({error.str(), 1});
                break;
            }
        }
    }

    if (optind < argc)
        m_inputFileName.assign(argv[optind]);
}

////////////////////////////////////////////////////////////////////////////////
#define USAGE_OPTION_LINE(shortOption, longOption, message) std::cout << "  -" << shortOption << ", --" << std::left << std::setw(22) << longOption  << "    " << message << std::endl
#define USAGE_OPTION_OTHERS_LINE(message)                   std::cout << std::left << std::setw(34) << ' ' << message << std::endl
void Options::usage(void)
{
    Options options;

    std::cout << "Usage : bnf2c [OPTIONS] [InputBnfFile]" << std::endl;

    std::cout << std::endl;
    USAGE_OPTION_LINE('h', "help"                  , "This help");

    std::cout << std::endl << "Parser states options :" << std::endl;
    USAGE_OPTION_LINE('s', "state-type"            , "Type used for generated states (default \"" << options.stateType << "\")");
    USAGE_OPTION_LINE('t', "top-state-code"        , "Code used to get the top state (default \"" << options.topState << "\")");
    USAGE_OPTION_LINE('p', "pop-state-code"        , "Code used to pop states from the stack (default \"" << options.popState << "\")");
    USAGE_OPTION_OTHERS_LINE(                        "The string \"" << Options::VAR_NB_STATES << "\" can be used to know how many states to pop");
    USAGE_OPTION_LINE('e', "error-state"           , "State number used to specify an error (default \"" << options.errorState << "\")");
    USAGE_OPTION_LINE('a', "accept-state"          , "State number used when parsing is done and the input is accepted (default \"" << options.acceptState << "\")");

    std::cout << std::endl << "Lexer options :" << std::endl;
    USAGE_OPTION_LINE('y', "token-type"            , "Type used for tokens (default \"" << options.tokenType << "\")");
    USAGE_OPTION_LINE('c', "shift-token-code"      , "Code used to move lexer to the next token (default \"" << options.shiftToken << "\")");
    USAGE_OPTION_LINE('r', "token-prefix"          , "Prefix of token (e.g. \"TokenType::\", default \"" << options.tokenPrefix << "\")");
    USAGE_OPTION_LINE('f', "end-of-input-token"    , "Name of the token use to specify end of input stream (default \"" << options.endOfInputToken << "\")");

    std::cout << std::endl << "Generated code options :" << std::endl;
    USAGE_OPTION_LINE('i', "intermediate-type"     , "Type used for generated intermediate type (default \"" << options.intermediateType << "\")");
    USAGE_OPTION_LINE('n', "parse-function"        , "Name of the generated parse function (default \"" << options.parseFunctionName << "\")");
    USAGE_OPTION_LINE('b', "branch-function"       , "Name of the generated branch function (default \"" << options.branchFunctionName << "\")");
    USAGE_OPTION_LINE('w', "throwed-exceptions"    , "Names of the exceptions throwed by generated functions (default no exceptions throwed)");
    USAGE_OPTION_LINE('d', "default-switch"        , "Generate a default statement in switch / case (default no default case)");
    USAGE_OPTION_LINE('u', "use-table-for-branches", "Use table instead of a function for branches (default use function)");

    std::cout << std::endl << "File :" << std::endl;
    USAGE_OPTION_LINE('o', "output OutputFile"     , "Specify the name of the output file (default to stdout)");
}

////////////////////////////////////////////////////////////////////////////////
std::istream & Options::inputStream(void) throw(CommandLineParsingError)
{
    if(m_inputFileName.empty())
        return std::cin;
    else
    {
        m_inputFileStream.open(m_inputFileName);

        if(m_inputFileStream.fail())
        {
            std::stringstream error;
            error << "Unable to open input file \"" << m_inputFileName << "\"";
            throw CommandLineParsingError({error.str(), 1});
        }

        return m_inputFileStream;
    }
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & Options::outputStream(void) throw(CommandLineParsingError)
{
    if(m_outputFileName.empty())
        return std::cout;
    else
    {
        m_outputFileStream.open(m_outputFileName);

        if(m_outputFileStream.fail())
        {
            std::stringstream error;
            error << "Unable to open output file \"" << m_outputFileName << "\"";
            throw CommandLineParsingError({error.str(), 1});
        }

        return m_outputFileStream;
    }
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Options & options)
{
    os << "topState               = \"" << options.topState               << "\"" << std::endl;
    os << "popState               = \"" << options.popState               << "\"" << std::endl;
    os << "errorState             = \"" << options.errorState             << "\"" << std::endl;
    os << "acceptState            = \"" << options.acceptState            << "\"" << std::endl;

    os << "stateType              = \"" << options.stateType              << "\"" << std::endl;
    os << "tokenType              = \"" << options.tokenType              << "\"" << std::endl;
    os << "intermediateType       = \"" << options.intermediateType       << "\"" << std::endl;

    os << "shiftToken             = \"" << options.shiftToken             << "\"" << std::endl;
    os << "tokenPrefix            = \"" << options.tokenPrefix            << "\"" << std::endl;
    os << "endOfInputToken        = \"" << options.endOfInputToken        << "\"" << std::endl;

    os << "parseFunctionName      = \"" << options.parseFunctionName      << "\"" << std::endl;
    os << "branchFunctionName     = \"" << options.branchFunctionName     << "\"" << std::endl;

    os << "defaultSwitchStatement = \"" << options.defaultSwitchStatement << "\"" << std::endl;
    os << "useTableForBranches    = \"" << options.useTableForBranches    << "\"" << std::endl;

    os << "tokenName              = \"" << options.tokenName              << "\"" << std::endl;
    os << "intermediateName       = \"" << options.intermediateName       << "\"" << std::endl;

    return os;
}
