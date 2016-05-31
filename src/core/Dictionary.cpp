////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#include "Dictionary.h"

#include <algorithm>
#include <iterator>

////////////////////////////////////////////////////////////////////////////////
void Dictionary::add(const std::string & name)
{
    if(name.length() > m_maxStringLength)
        m_maxStringLength = name.length();
    insert(name);
}

////////////////////////////////////////////////////////////////////////////////
void Dictionary::add(std::string && name)
{
    if(name.length() > m_maxStringLength)
        m_maxStringLength = name.length();
    insert(name);
}
////////////////////////////////////////////////////////////////////////////////
std::size_t Dictionary::index(const std::string & name) const
{
    return std::distance(begin(), std::find(begin(), end(), name));
}

////////////////////////////////////////////////////////////////////////////////
std::size_t Dictionary::getMaxSrtingLength(void) const
{
    return m_maxStringLength;
}

////////////////////////////////////////////////////////////////////////////////
bool Dictionary::contains(const std::string & name) const
{
    return (std::find(begin(), end(), name) != end());
}
