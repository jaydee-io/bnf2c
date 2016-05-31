////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "Options.h"
#include "Streams.h"
#include "bnf2c-parser/LexerBNF.h"
#include "bnf2c-parser/ParserBNF.h"
#include "core/Grammar.h"
#include "core/ParseTable.h"
#include "generator/ParserGenerator.h"

#include <string>
#include <iostream>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char ** argv)
{
    // Parse command line arguments
    Options cmdLineOptions;
    cmdLineOptions.parseArguments(argc, argv);

    // Check for command line errors
    if(!cmdLineOptions.errors.list.empty())
    {
        std::cerr << cmdLineOptions.errors << std::endl;
        Options::usage();
        return cmdLineOptions.errors.exitCode;
    }

    // Open input & output file
    Streams streams(cmdLineOptions.inputFileName, cmdLineOptions.outputFileName);

    // Read input file
    std::string inputBuffer;
    inputBuffer.assign((std::istreambuf_iterator<char>(streams.inputStream())), std::istreambuf_iterator<char>());

    // Start parser
    Grammar     grammar;
    LexerBNF    lexer(inputBuffer, streams.outputStream());
    ParserBNF   parser(lexer, grammar);

    // Find each "bnf2c" block and parse it
    while(lexer.moveToNextBnf2cBlock())
        parser.parseBnf2cBlock();

    // Command line options prevails over in file options
    Options options = parser.getInFileOptions();
    options << cmdLineOptions;

    // Check grammar
    grammar.check();
    if(!parser.errors.list.empty() || !grammar.errors.list.empty())
    {
        std::cerr << parser.errors;
        std::cerr << grammar.errors;
        return 1;
    }

    // Replace pseudo variable
    grammar.replacePseudoVariables(options);

    // Generate parser states
    ParseTable table(grammar, options);
    table.generateStates();
    table.check();
    if(!table.errors.list.empty())
    {
        std::cerr << table.errors;
        return 1;
    }

    // Output generated code at the end of output file
    ParserGenerator generator(table, grammar, options);
    generator.printTo(streams.outputStream());

    // Debug output
    if(options.debugLevel != DebugLevel::NONE)
        table.printDebug(std::cerr);

    return 0;
}
