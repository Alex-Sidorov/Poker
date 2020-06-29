#ifndef ABSTRACTREFEREE_H
#define ABSTRACTREFEREE_H

#include "card.h"

template<class T>
class AbstractReferee
{
public:
    AbstractReferee() {}
    virtual ~AbstractReferee(){}

    /**
     * @brief refereeing
     * @param hands(key is a player, value is his hand)
     * @return any winner or winners
     */
    virtual std::vector<T> refereeing(const std::map<T,Hand&> &hands) = 0;
};


#endif // ABSTRACTREFEREE_H
