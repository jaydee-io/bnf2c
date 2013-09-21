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
#include "LexerBNF.h"
#include "ParserBNF.h"
#include "Grammar.h"
#include "ParseTable.h"

#include <string>
#include <iostream>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char ** argv)
{
    try
    {
        // Parse command line arguments
        Options cmdLineOptions;
        cmdLineOptions.parseArguments(argc, argv);

        // Open input & output file
        std::istream & inputStream = cmdLineOptions.inputStream();
        std::ostream & outputStream = cmdLineOptions.outputStream();

        // Read input file
        std::string inputBuffer;
        inputBuffer.assign((std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>());

        // Start parser
        Options     options;
        Grammar     grammar;
        LexerBNF    lexer(inputBuffer, outputStream);
        ParserBNF   parser(lexer, options, grammar);

        // Find each "bnf2c" block and parse it
        while(lexer.moveToNextBnf2cBlock())
            parser.parseBnf2cBlock();

        // Command line options prevails over in file options
        options << cmdLineOptions;

        // Output generated code at the end of output file
        ParseTable table(grammar, options);
        grammar.replacePseudoVariables(options);
        table.checkGrammar();
        table.generateStates();
        table.generateBranchesCode(outputStream);
        outputStream << std::endl;
        table.generateParseCode(outputStream);

        // Debug output
        if(options.debugLevel != DebugLevel::NONE)
            table.printDebug(std::cerr);

        return 0;
    }
    catch(const CommandLineParsingError & e)
    {
        if(!e.message.empty())
            std::cerr << e.message << std::endl << std::endl;
        Options::usage();
        return e.exitCode;
    }
    catch(const ParsingError & error)
    {
        std::cerr << error << std::endl;
        return 1;
    }
    catch(const GeneratingError & e)
    {
        std::cerr << e.message << std::endl;
        return 1;
    }
}
