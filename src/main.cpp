////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "Options.h"
#include "LexerBNF.h"
#include "ParserBNF.h"
#include "Grammar.h"
#include "ParseTable.h"
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

    // Open input & output file
    std::istream & inputStream = cmdLineOptions.inputStream();
    std::ostream & outputStream = cmdLineOptions.outputStream();

    // Check for command line errors
    if(!cmdLineOptions.errors.list.empty())
    {
        std::cerr << cmdLineOptions.errors << std::endl;
        Options::usage();
        return cmdLineOptions.errors.exitCode;
    }

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
    generator.printTo(outputStream);

    // Debug output
    if(options.debugLevel != DebugLevel::NONE)
        table.printDebug(std::cerr);

    return 0;
}
