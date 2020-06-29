#include "uniformdistributionshuffle.h"

#include <chrono>
#include <random>
#include <algorithm>

UniformDistributionShuffle::UniformDistributionShuffle(){}

UniformDistributionShuffle::~UniformDistributionShuffle(){}

void UniformDistributionShuffle::shuffle(Hand& cards)
{
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::uniform_int_distribution<int> distribution(0,1000000);
    std::random_shuffle(cards.begin(),cards.end(),
    [&](int i)
    {
        return distribution(generator) % i;
    });
}
