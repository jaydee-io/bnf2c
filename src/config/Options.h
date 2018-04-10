////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef OPTIONS_H
#define OPTIONS_H
#include "ParameterizedString.h"
#include <string>
#include <vector>
#include <ostream>
#include <fstream>
#include <getopt.h>
#include "Errors.h"

struct Indenter
{
    std::string  string = "    ";
    unsigned int top    = 0;
    unsigned int indent = 0;

    Indenter & operator ++(int);
    Indenter & operator --(int);
    Indenter & operator ++(void);
    Indenter & operator --(void);

    bool operator !=(const Indenter & indenter) const;
};

std::ostream & operator <<(std::ostream & os, const Indenter & indenter);


enum class DebugLevel
{
    NONE,
    GENERATOR,
    PARSER,
    LEXER
};

struct Options
{
    public :
        static Options           DEFAULT;
        static const std::string VERSION;

        // Parser options
        std::string         parserType      = "LALR1";
        std::string         stateType       = "int";
        std::string         topState        = "topState()";
        ParameterizedString popState        = "popStates(<NB_STATES>);";
        std::string         errorState      = "-1";
        std::string         acceptState     = "-2";

        std::string         valueType           = "ValueType";
        ParameterizedString pushValue           = "pushValue(<VALUE>);";
        ParameterizedString popValues           = "popValues(<NB_VALUES>);";
        ParameterizedString getValue            = "getValue(<VALUE_IDX>).<TYPE>";
        ParameterizedString valueAsToken        = "<VALUE>.<TYPE>";
        ParameterizedString valueAsIntermediate = "<VALUE>.<TYPE>";
        ParameterizedString defaultAction       = "$$ = $1;";

        // Lexer options
        std::string         tokenType       = "int";
        std::string         shiftToken      = "shiftToken();";
        std::string         tokenPrefix     = "";
        ParameterizedString getTypeOfToken  = "<TOKEN>";
        std::string         endOfInputToken = "END_OF_INPUT";

        // Generated code options
        std::string         intermediateType    = "int";
        std::string         parseFunctionName   = "parse";
        std::string         branchFunctionName  = "branch";
        std::string         throwedExceptions   = "";

        bool                defaultSwitchStatement = false;
        bool                useTableForBranches    = false;

        std::string         inputFileName;
        std::string         outputFileName;

        // Internal
        std::string         tokenName        = "yytoken";
        std::string         intermediateName = "intermediate";

        Indenter            indent;

        DebugLevel          debugLevel = DebugLevel::NONE;

        Errors<CommandLineParsingError> errors;

    public :
        void parseArguments(int argc, char ** argv);
        static void usage(void);

        // Overwrite option if the corresponding option in "options" is not default
        Options & operator <<(const Options & options);

    protected :
        static const struct option            OPTIONS [];
        static const std::vector<std::string> OPTIONS_TEXT [];
};

#endif /* OPTIONS_H */
