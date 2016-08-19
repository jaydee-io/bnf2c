////////////////////////////////////////////////////////////////////////////////
//                                    BNF2C
//
// This file is distributed under the 4-clause Berkeley Software Distribution
// License. See LICENSE for details.
////////////////////////////////////////////////////////////////////////////////
#ifndef _LALR1STATE_H_
#define _LALR1STATE_H_
#include "core/LR1/LR1State.h"

class LALR1State : public LR1State
{
    public :
        bool isMergeableWith(const Ptr & state) override;
        void merge(Ptr & state) override;
};

#endif /* _LALR1STATE_H_ */
