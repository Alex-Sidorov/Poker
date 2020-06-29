#ifndef ABSTRACTCARDSSHUFFLE_H
#define ABSTRACTCARDSSHUFFLE_H

#include "card.h"

class AbstractCardsShuffle
{
public:
    AbstractCardsShuffle();
    virtual ~AbstractCardsShuffle();

    virtual void shuffle_cards(Hand& cards) = 0;

};

#endif // ABSTRACTCARDSSHUFFLE_H
