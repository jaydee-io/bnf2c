#include "LexerBNF.h"
#include "ParserBNF.h"
#include "ParsingTable.h"

#include "Dictionnary.h"

#include <string>
#include <fstream>
#include <iostream>
#include <cstring>

int main(int argc, char ** argv)
{
    // Parse arguments
    if(argc < 2)
    {
        std::cout << "Usage : " << argv[0] << " FileBNF" << std::endl;
        return 1;
    }

    // Read input file
    std::ifstream inputFile(argv[1]);
    std::string   input((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

    // Start parser
    LexerBNF        lexer(input.c_str());
    ParserBNF       parser(lexer);
    ParsingTable    table;
    std::string     source;


    // Find each "bnf2c" block
    while(lexer.nextBnf2c(source))
    {
        parser.outputFormatter.outputStream << source << std::endl;

        try
        {
            parser.parse();
        }
        catch(const ParsingError & error)
        {
            std::cerr << error;
            return 1;
        }
    }

    if(!parser.grammar.rules.empty())
    {
        table.generateStates(parser.grammar);
        table.output(parser.outputFormatter, parser.grammar);
    }

    parser.outputFormatter.outputStream << source << std::endl;

    // Verbose
    if((argc == 3) && (std::strcmp(argv[2], "-v") == 0))
    {
        table.outputDebug(std::cerr);
        std::cerr << "#Rules = "         << parser.grammar.rules.size()         << std::endl;
        std::cerr << "#Terminals = "     << parser.grammar.terminals.size()     << std::endl;
        std::cerr << "#Intermediates = " << parser.grammar.intermediates.size() << std::endl;
    }

    return 0;
}
