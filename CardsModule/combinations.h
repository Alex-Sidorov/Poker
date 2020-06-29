#ifndef COMBINATIONS_H
#define COMBINATIONS_H

#include <map>

enum class Combination
{
    KICKER,
    ONE_PAIR,
    TWO_PAIR,
    THREE_OF_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_KIND,
    STRAIGHT_FLUSH
};

const std::map<Combination, uint64_t> COMBINATION_MULS
{
    {Combination::KICKER,        1},
    {Combination::ONE_PAIR,      100},
    {Combination::TWO_PAIR,      10000},
    {Combination::THREE_OF_KIND, 1000000},
    {Combination::STRAIGHT,      100000000},
    {Combination::FLUSH,         10000000000},
    {Combination::FULL_HOUSE,    1000000000000},
    {Combination::FOUR_OF_KIND,  100000000000000},
    {Combination::STRAIGHT_FLUSH,10000000000000000}
};

#endif // COMBINATIONS_H
