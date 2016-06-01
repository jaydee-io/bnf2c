////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _PARAMETERIZED_STRING_H_
#define _PARAMETERIZED_STRING_H_
#include <string>

namespace Vars
{
    extern const std::string NB_STATES;
    extern const std::string VALUE;
    extern const std::string NB_VALUES;
    extern const std::string VALUE_IDX;
    extern const std::string EXTERNAL_RETURN;
    extern const std::string RETURN;
    extern const std::string TOKEN;
    extern const std::string TYPE;
};

class ParameterizedString
{
    public :
        ParameterizedString(const std::string & str) : m_str(str) { }
        ParameterizedString(const char * str) : m_str(str) { }

        ParameterizedString & operator =(const std::string & str) { m_str = str; return *this; }
        ParameterizedString & operator =(const char * str) { m_str = str; return *this; }

        bool operator ==(const ParameterizedString & str) const { return m_str == str.m_str; }
        bool operator !=(const ParameterizedString & str) const { return m_str != str.m_str; }

        ParameterizedString replaceParam(const std::string & pattern, const std::string & replacement) const;

        const std::string & toString(void) const { return m_str; }

    protected :
        std::string m_str;
};

inline std::ostream & operator <<(std::ostream & os, const ParameterizedString & str)
{
    os << str.toString();
    return os;
}

#endif /* _PARAMETERIZED_STRING_H_ */

