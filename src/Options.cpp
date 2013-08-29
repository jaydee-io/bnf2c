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
#include "OutputFormatter.h"

const std::string OutputFormatter::VAR_NB_STATES("<NB_STATES>");

////////////////////////////////////////////////////////////////////////////////
Indenter & Indenter::operator ++(int)
{
    indent++;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
Indenter & Indenter::operator --(int)
{
    indent--;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
Indenter & Indenter::operator ++(void)
{
    indent++;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
Indenter & Indenter::operator --(void)
{
    indent--;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Indenter & indenter)
{
    for(unsigned int i = 0; i < (indenter.top + indenter.indent); i++)
        os << indenter.string;

    return os;
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const OutputFormatter & outputFormatter)
{
    os << "topState               = \"" << outputFormatter.topState               << "\"" << std::endl;
    os << "popState               = \"" << outputFormatter.popState               << "\"" << std::endl;
    os << "errorState             = \"" << outputFormatter.errorState             << "\"" << std::endl;
    os << "acceptState            = \"" << outputFormatter.acceptState            << "\"" << std::endl;

    os << "stateType              = \"" << outputFormatter.stateType              << "\"" << std::endl;
    os << "tokenType              = \"" << outputFormatter.tokenType              << "\"" << std::endl;
    os << "intermediateType       = \"" << outputFormatter.intermediateType       << "\"" << std::endl;

    os << "shiftToken             = \"" << outputFormatter.shiftToken             << "\"" << std::endl;
    os << "tokenPrefix            = \"" << outputFormatter.tokenPrefix            << "\"" << std::endl;
    os << "endOfInputToken        = \"" << outputFormatter.endOfInputToken        << "\"" << std::endl;

    os << "parseFunctionName      = \"" << outputFormatter.parseFunctionName      << "\"" << std::endl;
    os << "branchFunctionName     = \"" << outputFormatter.branchFunctionName     << "\"" << std::endl;

    os << "defaultSwitchStatement = \"" << outputFormatter.defaultSwitchStatement << "\"" << std::endl;
    os << "useTableForBranches    = \"" << outputFormatter.useTableForBranches    << "\"" << std::endl;

    os << "tokenName              = \"" << outputFormatter.tokenName              << "\"" << std::endl;
    os << "intermediateName       = \"" << outputFormatter.intermediateName       << "\"" << std::endl;

    os << "indent.string          = \"" << outputFormatter.indent.string          << "\"" << std::endl;
    os << "indent.top             = \"" << outputFormatter.indent.top             << "\"" << std::endl;

    return os;
}
