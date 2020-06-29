#ifndef UNIFORMDISTRIBUTIONSHUFFLE_H
#define UNIFORMDISTRIBUTIONSHUFFLE_H

#include "abstractcardsshuffle.h"

class UniformDistributionShuffle : public AbstractCardsShuffle
{
public:
    UniformDistributionShuffle();
    virtual ~UniformDistributionShuffle();

    virtual void shuffle(Hand& cards);
};

#endif // UNIFORMDISTRIBUTIONSHUFFLE_H
