////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef ALGOS_H
#define ALGOS_H
#include <utility>

// Utilities algorithm
template<typename InputIterator, typename Func>
void for_each(const std::pair<InputIterator, InputIterator> & range, Func func)
{
    auto first = range.first;
    auto last = range.second;

    while(first != last)
        func(*first++);
}

template<typename Container1, typename Container2, typename Func>
void for_each_pair(Container1 c1, Container2 c2, Func func)
{
    auto first1 = std::begin(c1);
    auto last1 = std::end(c1);
    auto first2 = std::begin(c2);
    auto last2 = std::end(c2);

    while(first1 != last1 && first2 != last2)
        func(*first1++, *first2++);
}

template<typename Container1, typename Container2, typename Predicate>
bool all_of_pairs(Container1 c1, Container2 c2, Predicate pred)
{
    auto first1 = std::begin(c1);
    auto last1 = std::end(c1);
    auto first2 = std::begin(c2);
    auto last2 = std::end(c2);

    while(first1 != last1 && first2 != last2)
    {
        if(!pred(*first1++, *first2++))
            return false;
    }
    return true;
}

#endif /* ALGOS_H */

