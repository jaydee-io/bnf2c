////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef LALR1STATE_H
#define LALR1STATE_H
#include "core/LR1/LR1State.h"

class LALR1State : public LR1State
{
    public :
        virtual ~LALR1State(void) = default;

        bool isMergeableWith(const Ptr & state) override;
        void merge(Ptr & state) override;
};

#endif /* LALR1STATE_H */
