#include "ParameterizedString.h"

namespace Vars
{
    const std::string NB_STATES      ("<NB_STATES>");
    const std::string VALUE          ("<VALUE>");
    const std::string VALUE_IDX      ("<VALUE_IDX>");
    const std::string NB_VALUES      ("<NB_VALUES>");
    const std::string EXTERNAL_RETURN("$$");
    const std::string RETURN         ("yylval");
    const std::string TOKEN          ("<TOKEN>");
    const std::string TYPE           ("<TYPE>");
};

ParameterizedString ParameterizedString::replaceParam(const std::string & pattern, const std::string & replacement) const
{
    // Check that replacement string doesn't contains the pattern to avoid infinite loop
    if(replacement.find(pattern) != std::string::npos)
        return *this;

    // Replace each occurrence
    ParameterizedString replacedStr(m_str);
    std::size_t pos = 0;

    while((pos = replacedStr.m_str.find(pattern, pos)) != std::string::npos)
        replacedStr.m_str.replace(pos, pattern.size(), replacement);

    return replacedStr;
}

