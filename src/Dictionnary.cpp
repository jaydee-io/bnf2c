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
#include "Dictionnary.h"

#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
size_t Dictionnary::size(void) const
{
    return m_dictionnary.size();
}

////////////////////////////////////////////////////////////////////////////////
Dictionnary::Index Dictionnary::add(const std::string & name)
{
    return m_dictionnary.insert(name).first;
}

////////////////////////////////////////////////////////////////////////////////
Dictionnary::Index Dictionnary::add(std::string && name)
{
    return m_dictionnary.insert(name).first;
}

////////////////////////////////////////////////////////////////////////////////
const std::string & Dictionnary::operator [](const Dictionnary::Index & index) const
{
    return *index;
}

////////////////////////////////////////////////////////////////////////////////
Dictionnary::Index Dictionnary::operator [](const std::string & name) const
{
    return m_dictionnary.find(name);
}

////////////////////////////////////////////////////////////////////////////////
size_t Dictionnary::index(const Index & index) const
{
    return std::distance(m_dictionnary.begin(), index);
}

////////////////////////////////////////////////////////////////////////////////
Dictionnary::Index Dictionnary::begin(void) const
{
    return m_dictionnary.begin();
}

////////////////////////////////////////////////////////////////////////////////
Dictionnary::Index Dictionnary::end(void) const
{
    return m_dictionnary.end();
}

////////////////////////////////////////////////////////////////////////////////
std::ostream & operator <<(std::ostream & os, const Dictionnary::Index & index)
{
    os << *index;

    return os;
}
