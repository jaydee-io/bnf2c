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
#ifndef __DICTIONNARY_H__
#define __DICTIONNARY_H__
#include <string>
#include <unordered_set>
#include <functional>
#include <iostream>

class Dictionnary
{
    protected :
        typedef std::unordered_set<std::string> InternalType;

    public :
        typedef InternalType::const_iterator    Index;

    public :
        std::size_t size(void) const;

        Index add(const std::string & name);
        Index add(std::string && name);

        const std::string & operator [](const Index & index) const;
        Index               operator [](const std::string & name) const;

        std::size_t index(const Index & index) const;

        Index begin(void) const;
        Index end(void) const;

        std::size_t getMaxSrtingLength(void) const;

        friend std::ostream & operator <<(std::ostream & os, const Dictionnary & dictionnary);

    protected :
        InternalType    m_dictionnary;
        std::size_t     m_maxStringLength = 0;
};

////////////////////////////////////////////////////////////////////////////////
template<>
struct std::hash<Dictionnary::Index>
{
    size_t operator ()(const ::Dictionnary::Index & index) const
    {
        std::hash<std::string> hasher;

        return hasher(*index);
    }
};

std::ostream & operator <<(std::ostream & os, const Dictionnary::Index & index);

#endif /* __DICTIONNARY_H__ */
