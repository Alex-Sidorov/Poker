#include <iostream>
#include <CardsModule/referee.h>
using namespace std;

constexpr size_t COUNT = 7;

Hand get_one_pair_combination()
{
    if(COUNT < 2)
    {
        return {};
    }

    srand(time(0));

    Hand res;

    VALUES_CARDS value = VALUES_CARDS::ACE_AS_ONE;
    while(value == VALUES_CARDS::ACE_AS_ONE)
    {
        value = VALUES[rand() % VALUES.size()];
    }
    res.push_back({value, SUITS[rand() % SUITS.size()]});
    res.push_back({value, res[0].suit == SUIT::CLUBS ? SUIT::HEARTS : SUIT::CLUBS});

    for(size_t i = 0; i < COUNT - 1; i++)
    {
        Card temp = {VALUES[rand() % VALUES.size()], SUITS[rand() % SUITS.size()]};
        if(std::find_if(res.begin(),res.end(),
        [temp](const Card &v){return temp.value == v.value;}) == res.end())
        {
            res.push_back(temp);
        }
    }

    return res;
}

Hand get_four_of_kind_combination()
{
    if(COUNT < 4)
    {
        return {};
    }

    srand(time(0));

    Hand res;

    VALUES_CARDS value = VALUES_CARDS::ACE;
    /*while(value == VALUES_CARDS::ACE_AS_ONE)
    {
        value = VALUES[rand() % VALUES.size()];
    }*/
    for(size_t i = 0; i < SUITS.size(); ++i)
    {
        res.push_back({value,SUITS[i]});
    }


    while(res.size() != COUNT)
    {
        while(value == VALUES_CARDS::ACE_AS_ONE ||
              std::find_if(res.begin(), res.end(),[&value](Card v){return value == v.value;}) != res.end())
        {
            value = VALUES[rand() % VALUES.size()];
        }
        res.push_back({value,SUITS[rand() % SUITS.size()]});
    }
    return res;
}

Hand get_flush()
{
    if(COUNT < 5)
    {
        return {};
    }

    srand(time(0));

    VALUES_CARDS value = VALUES_CARDS::ACE;
    Hand res;
    while(res.size() != COUNT)
    {
        while(std::find_if(res.begin(), res.end(),[&value](Card v){return value == v.value;}) != res.end())
        {
            value = VALUES[rand() % VALUES.size()];
            while(value == VALUES_CARDS::ACE_AS_ONE)
            {
                value = VALUES[rand() % VALUES.size()];
            }
        }
        res.push_back({value,SUITS[0]});
    }
    return res;
}

Hand get_straigth()
{
    if(COUNT < 5)
    {
        return {};
    }

    srand(time(0));

    Hand res;
    for (size_t i = 0; i < COUNT; ++i)
    {
        res.push_back({VALUES[i + 1], SUITS[(i + 1) % SUITS.size()]});
    }
    return res;
}

Hand get_low_straigth()
{
    if(COUNT < 5)
    {
        return {};
    }

    srand(time(0));

    Hand res;
    for (size_t i = 0; i < 4; ++i)
    {
        res.push_back({VALUES[i + 1], SUITS[(i + 1) % SUITS.size()]});
    }
    res.push_back({VALUES.back(),SUITS[0]});

    VALUES_CARDS value = VALUES[rand() % VALUES.size()];
    while(res.size() != COUNT)
    {
        while(std::find_if(res.begin(), res.end(),[&value](Card v){return value == v.value;}) != res.end() && value == VALUES_CARDS::ACE_AS_ONE)
        {
            value = VALUES[rand() % VALUES.size()];
        }
        res.push_back({value,SUITS[0]});
    }

    return res;
}

Hand get_rand_combination(size_t count)
{
    srand(time(0));
    Hand res;
    for(size_t i = 0; i < count; i++)
    {
        res.push_back({VALUES[rand() % (VALUES.size() - 1)], SUITS[rand() % SUITS.size()]});
    }
    return res;
}

Hand get_full_haus()
{
    Hand res;
    VALUES_CARDS value = VALUES_CARDS::ACE;
    for(size_t i = 0; i < SUITS.size() - 1; ++i)
    {
        res.push_back({value,SUITS[i]});
    }
    value = VALUES_CARDS::TWO;
    for(size_t i = 0; i < SUITS.size() - 2; ++i)
    {
        res.push_back({value,SUITS[i]});
    }
    while(res.size() != COUNT)
    {
        while(value == VALUES_CARDS::ACE_AS_ONE ||
              std::find_if(res.begin(), res.end(),[&value](Card v){return value == v.value;}) != res.end())
        {
            value = VALUES[rand() % VALUES.size()];
        }
        res.push_back({value,SUITS[rand() % SUITS.size()]});
    }
    return res;
}

Hand get_straigth_flush()
{
    srand(time(0));

    Hand res;
    for (size_t i = 0; i < COUNT; ++i)
    {
        res.push_back({VALUES[i + 1], SUITS[0]});
    }
    return res;
}

int main()
{
    Referee<int> referee;

    std::map<int,Hand> hands;
    hands[0] = get_straigth();
    hands[1] = get_flush();

    auto res = referee.refereeing(hands);

    return 0;
}
