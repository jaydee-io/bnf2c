////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "PrettyPrinters.h"
#include "config/Options.h"

////////////////////////////////////////////////////////////////////////////////
#define DISPLAY_OPTION(option) os << #option << " = \"" << options.option << '"' << std::endl
std::ostream & operator <<(std::ostream & os, const Options & options)
{
    // Parser options
    DISPLAY_OPTION(parserType         );
    DISPLAY_OPTION(stateType          );
    DISPLAY_OPTION(topState           );
    DISPLAY_OPTION(popState           );
    DISPLAY_OPTION(errorState         );
    DISPLAY_OPTION(acceptState        );
    DISPLAY_OPTION(valueType          );
    DISPLAY_OPTION(pushValue          );
    DISPLAY_OPTION(popValues          );
    DISPLAY_OPTION(getValue           );
    DISPLAY_OPTION(valueAsToken       );
    DISPLAY_OPTION(valueAsIntermediate);
    DISPLAY_OPTION(defaultAction      );

    // Lexer options
    DISPLAY_OPTION(tokenType      );
    DISPLAY_OPTION(shiftToken     );
    DISPLAY_OPTION(tokenPrefix    );
    DISPLAY_OPTION(getTypeOfToken );
    DISPLAY_OPTION(endOfInputToken);

    // Generated code options
    DISPLAY_OPTION(intermediateType  );
    DISPLAY_OPTION(parseFunctionName );
    DISPLAY_OPTION(branchFunctionName);

    DISPLAY_OPTION(defaultSwitchStatement);
    DISPLAY_OPTION(useTableForBranches   );

    DISPLAY_OPTION(tokenName       );
    DISPLAY_OPTION(intermediateName);

    return os;
}
