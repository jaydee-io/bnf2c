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
#include "ParsingTable.h"

#include <string>
#include <iostream>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char ** argv)
{
    try
    {
        // Parse command line arguments
        Options options;
        options.parseArguments(argc, argv);

        // Open input & output file
        std::istream & inputStream = options.inputStream();
        std::ostream & outputStream = options.outputStream();

        // Read input file
        std::string inputBuffer;
        inputBuffer.assign((std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>());

        // Start parser
        LexerBNF        lexer(inputBuffer, outputStream);
        ParserBNF       parser(lexer, options);

        // Find each "bnf2c" block
        while(lexer.moveToNextBnf2cBlock())
        {
            try
            {
                parser.parseBnf2cBlock();
            }
            catch(const ParsingError & error)
            {
                std::cerr << error;
                return 1;
            }
        }

        // Output generated code
        if(!parser.grammar.rules.empty())
        {
            ParsingTable table;
            table.generateStates(parser.grammar);
            table.outputTable(outputStream, options, parser.grammar);
        }

        return 0;
    }
    catch(const CommandLineParsingError & e)
    {
        if(!e.message.empty())
            std::cerr << e.message << std::endl << std::endl;
        Options::usage();
        return e.exitCode;
    }
}
