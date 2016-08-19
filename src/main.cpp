////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "Streams.h"
#include "config/Options.h"
#include "bnf2c-parser/LexerBNF.h"
#include "bnf2c-parser/ParserBNF.h"
#include "core/Grammar.h"
#include "core/Parser.h"
#include "core/LR0/LR0Parser.h"
#include "core/LR1/LR1Parser.h"
#include "core/LALR1/LALR1Parser.h"
#include "generator/ParserGenerator.h"

#include <string>
#include <iostream>
#include <cstring>
#include <memory>

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
    LexerBNF    bnfLexer(inputBuffer, streams.outputStream());
    ParserBNF   bnfParser(bnfLexer, grammar);

    // Find each "bnf2c" block and parse it
    while(bnfLexer.moveToNextBnf2cBlock())
        bnfParser.parseBnf2cBlock();

    // Command line options prevails over in file options
    Options options = bnfParser.getInFileOptions();
    options << cmdLineOptions;

    // Check grammar
    grammar.check();
    if(!bnfParser.errors.list.empty() || !grammar.errors.list.empty())
    {
        std::cerr << bnfParser.errors;
        std::cerr << grammar.errors;
        return 1;
    }

    // Replace pseudo variable
    grammar.replacePseudoVariables(options);

    // Generate parser states
    std::unique_ptr<Parser> parser;
    if(options.parserType == "LR0")
        parser = std::make_unique<LR0Parser>(grammar, options);
    else if(options.parserType == "LR1")
        parser = std::make_unique<LR1Parser>(grammar, options);
    else if(options.parserType == "LALR1")
        parser = std::make_unique<LALR1Parser>(grammar, options);
    parser->generateStates();
    parser->check();
    if(!parser->errors.list.empty())
    {
        std::cerr << parser->errors;
        return 1;
    }

    // Output generated code at the end of output file
    ParserGenerator generator(*parser, grammar, options);
    generator.printTo(streams.outputStream());

    // Debug output
    if(options.debugLevel != DebugLevel::NONE)
        parser->printDebug(std::cerr);

    return 0;
}
