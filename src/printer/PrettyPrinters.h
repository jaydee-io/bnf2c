////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _PRETTY_PRINTER_H_
#define _PRETTY_PRINTER_H_
#include <ostream>

// Core objects
struct Symbol;
struct Item;
class Rule;
class ParserState;
class Parser;
class Grammar;

std::ostream & operator <<(std::ostream & os, const Symbol & symbol);
std::ostream & operator <<(std::ostream & os, const Item & item);
std::ostream & operator <<(std::ostream & os, const Rule & rule);
std::ostream & operator <<(std::ostream & os, const Grammar & grammar);
std::ostream & operator <<(std::ostream & os, const ParserState & state);
std::ostream & operator <<(std::ostream & os, const Parser & parser);



// bnf2c parser
enum class TokenType;
class Token;
std::ostream & operator<<(std::ostream & os, const TokenType tokenType);
std::ostream & operator<<(std::ostream & os, const Token & token);



// Errors
struct CommandLineParsingError;
struct ParsingError;
struct GeneratingError;
template<class ErrorType>
struct Errors;

std::ostream & operator <<(std::ostream & os, const CommandLineParsingError & error);
std::ostream & operator <<(std::ostream & os, const ParsingError & error);
std::ostream & operator <<(std::ostream & os, const GeneratingError & error);

template<class ErrorType>
std::ostream & operator <<(std::ostream & os, const Errors<ErrorType> & errors)
{
    for(const ErrorType & error : errors.list)
        os << error;

    return os;
}



// Options
struct Options;

std::ostream & operator <<(std::ostream & os, const Options & options);



// Utilities
namespace detail {
template<class Container>
struct SequenceEmitter
{
    SequenceEmitter(const Container & container, std::string separator)
    : container(container), sep(std::move(separator))
    {}

    std::ostream & writeTo(std::ostream & os) const
    {
        auto first = std::begin(container);
        auto last = std::end(container);

        if(first != last)
        {
            os << *first++;

            while(first != last)
                os << sep << *first++;
        }

        return os;
    }

private:
    const Container & container;
    std::string sep;
};

template<class Container>
std::ostream & operator<<(std::ostream & os, const SequenceEmitter<Container> & se)
{
    return se.writeTo(os);
}
} /* Utilities namesapce */

template<class Container>
detail::SequenceEmitter<Container> separate_elems(const Container & container, std::string separator = ", ")
{
    return { container, std::move(separator) };
}

#endif /* _PRETTY_PRINTER_H_ */

