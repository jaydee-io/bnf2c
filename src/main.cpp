#include "LexerBNF.h"
#include "ParserBNF.h"
#include "ParsingTable.h"

#include "Dictionnary.h"

#include <string>
#include <fstream>
#include <iostream>

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

    table.generate(parser.grammar);
    table.output(parser.outputFormatter, parser.grammar);

    parser.outputFormatter.outputStream << source << std::endl;

    return 0;
}
