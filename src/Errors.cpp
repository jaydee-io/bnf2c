////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "Errors.h"

////////////////////////////////////////////////////////////////////////////////
ParsingError::ParsingError(const LexerState & lexerState, unsigned long tokenLength, std::string && message)
: lineString(lexerState.getCurrentLine()),
  line(lexerState.getLine()),
  column(lexerState.getColumn()),
  nTabs(lexerState.getTabulations()),
  tokenLength(tokenLength),
  message(std::forward<std::string>(message))
{ }

