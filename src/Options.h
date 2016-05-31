////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _OPTIONS_H_
#define _OPTIONS_H_
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
        static Options              DEFAULT;
        static const std::string    VAR_NB_STATES;
        static const std::string    VAR_VALUE;
        static const std::string    VAR_NB_VALUES;
        static const std::string    VAR_VALUE_IDX;
        static const std::string    VAR_EXTERNAL_RETURN;
        static const std::string    VAR_RETURN;
        static const std::string    VAR_TOKEN;
        static const std::string    VAR_TYPE;
        static const std::string    VERSION;

        // Parser options
        std::string     stateType           = "int";
        std::string     topState            = "topState()";
        std::string     popState            = "popStates(<NB_STATES>);";
        std::string     errorState          = "-1";
        std::string     acceptState         = "-2";

        std::string     valueType           = "ValueType";
        std::string     pushValue           = "pushValue(<VALUE>);";
        std::string     popValues           = "popValues(<NB_VALUES>);";
        std::string     getValue            = "getValue(<VALUE_IDX>).<TYPE>";
        std::string     valueAsToken        = "<VALUE>.<TYPE>";
        std::string     valueAsIntermediate = "<VALUE>.<TYPE>";

        // Lexer options
        std::string     tokenType           = "int";
        std::string     shiftToken          = "shiftToken();";
        std::string     tokenPrefix         = "";
        std::string     getTypeOfToken      = "<TOKEN>";
        std::string     endOfInputToken     = "END_OF_INPUT";

        // Generated code options
        std::string     intermediateType    = "int";
        std::string     parseFunctionName   = "parse";
        std::string     branchFunctionName  = "branch";
        std::string     throwedExceptions   = "";

        bool            defaultSwitchStatement = false;
        bool            useTableForBranches    = false;

        std::string     inputFileName;
        std::string     outputFileName;

        // Internal
        std::string                     tokenName        = "yytoken";
        std::string                     intermediateName = "intermediate";

        Indenter                        indent;

        DebugLevel                      debugLevel = DebugLevel::NONE;
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

std::ostream & operator <<(std::ostream & os, const Options & options);

#endif /* _OPTIONS_H_ */
