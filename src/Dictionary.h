////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_
#include <string>
#include <unordered_set>

class Dictionary : public std::unordered_set<std::string>
{
    public :
        void add(const std::string & name);
        void add(std::string && name);

        std::size_t index(const std::string & name) const;

        std::size_t getMaxSrtingLength(void) const;

        bool contains(const std::string & name) const;

    protected :
        std::size_t     m_maxStringLength = 0;
};

#endif /* _DICTIONARY_H_ */
