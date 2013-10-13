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
#ifndef __OPTIONS_H__
#define __OPTIONS_H__
#include <string>
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
        static const std::string    VERSION;

        // Parser options
        std::string     stateType          = "int";
        std::string     topState           = "topState()";
        std::string     popState           = "popStates(<NB_STATES>)";
        std::string     errorState         = "-1";
        std::string     acceptState        = "-2";

        std::string     valueType          = "ValueType";
        std::string     pushValue          = "pushValue(<VALUE>)";
        std::string     popValues          = "popValues(<NB_VALUES>)";
        std::string     getValue           = "getValue(<VALUE_IDX>)";

        // Lexer options
        std::string     tokenType          = "int";
        std::string     tokenUnionName     = "token";
        std::string     shiftToken         = "shiftToken()";
        std::string     tokenPrefix        = "";
        std::string     getTypeOfToken     = "<TOKEN>";
        std::string     endOfInputToken    = "END_OF_INPUT";

        // Generated code options
        std::string     intermediateType   = "int";
        std::string     parseFunctionName  = "parse";
        std::string     branchFunctionName = "branch";
        std::string     throwedExceptions  = "";

        bool            defaultSwitchStatement = false;
        bool            useTableForBranches    = false;

        // Internal
        std::string                     tokenName        = "yytoken";
        std::string                     intermediateName = "intermediate";

        Indenter                        indent;

        DebugLevel                      debugLevel = DebugLevel::NONE;
        Errors<CommandLineParsingError> errors;

    public :
        void parseArguments(int argc, char ** argv);
        static void usage(void);

        std::istream & inputStream(void);
        std::ostream & outputStream(void);

        // Overwrite option if the corresponding option in "options" is not default
        Options & operator <<(const Options & options);

    protected :
        static const struct option LONG_OPTIONS [];
        static const char          SHORT_OPTIONS [];

        std::string     m_inputFileName;
        std::string     m_outputFileName;

        std::ifstream   m_inputFileStream;
        std::ofstream   m_outputFileStream;
};

std::ostream & operator <<(std::ostream & os, const Options & options);

#endif /* __OPTIONS_H__ */
