#ifndef REFEREE_H
#define REFEREE_H

#include "abstractreferee.h"
#include "combinations.h"

#include <algorithm>
#include <future>

template<class T>
class Referee : public AbstractReferee<T>
{

public:
    Referee(){}
    virtual ~Referee(){}

    virtual std::vector<T> refereeing(const std::map<T,Hand&> &hands)
    {
        std::map<T,std::future<uint64_t>> scores;
        for (auto it = hands.begin(); it != hands.end(); it++)
        {
            auto &player = it.key();
            auto &hand = it.value(); 
            scores.insert(player,std::async( [&] { this->get_score(hand); } ) );
        }

        uint64_t temp = 0;

        std::vector<T> winner;
        for (auto it = winner.begin(); it != winner.end(); it++)
        {
            auto &player = it.key();
            auto &score = it.value();

            if(temp == score)
            {
                winner.push_back(player);
            }
            else if(temp < score)
            {
                winner.clear();
                winner.push_back(player);
                temp = score;
            }
        }
        return winner;
    }

private:

    std::pair<Combination,uint64_t> get_score(const Hand& hand)
    {

        std::sort(hand.begin(),hand.end(),[&](Card& first, Card& second)
        {
            auto &first_score = PRIORITY_CARDS.find(first.value)->second;
            auto &second_score = PRIORITY_CARDS.find(second.value)->second;
            return first_score < second_score;
        });

        auto flush_res = is_flush(hand);
        if(flush_res.first)
        {
            auto begin_flush = flush_res.second;
            Hand temp;            
            std::copy(hand.begin() + begin_flush, hand.begin() + begin_flush + 5, temp);

            auto res_straight = is_straight(temp);
            if(res_straight)
            {
                auto value = res_straight * COMBINATION_MULS.find(Combination::STRAIGHT_FLUSH)->second;
                return {Combination::STRAIGHT_FLUSH, value};
            }
            else
            {
                auto value = flush_res.first * COMBINATION_MULS.find(Combination::FLUSH)->second;
                return {Combination::FLUSH, value};
            }
        }

        auto res_straight = is_straight(hand);
        if(res_straight)
        {
            auto value = res_straight * COMBINATION_MULS.find(Combination::STRAIGHT)->second;
            return {Combination::STRAIGHT, value};
        }


        uint64_t res = 0;
        int index = hand.size();
        for(int index = hand.size() - 1, count = 0; index >= 0 && count < 5; --index, count++)
        {
            res += PRIORITY_CARDS.find(hand[index].value)->second;
        }
        return {Combination::KICKER, res};
    }

    std::pair<uint64_t,int> is_flush(const Hand& hand)
    {        
        for(auto& suit : SUITS)
        {
            auto count_cards = std::count_if(hand.begin(),hand.end(),[&](Card &value) { return value.suit == suit; });
            if(count_cards >= 5)
            {
                uint64_t res = 0;
                int index = hand.size();
                short int count = 5;
                while(count)
                {
                    if(hand[index].suit == suit)
                    {
                        auto &score = PRIORITY_CARDS.find(hand[index].value)->second;
                        res += score;
                        --count;
                    }
                    --index;
                }
                return {res,index+1};
            }
        }
        return {0,0};
    }

    uint64_t is_straight(const Hand& hand)
    {        
        if(hand.size())
        {
            int count = 1;
            uint64_t res = PRIORITY_CARDS.find(hand.back().value)->second;
            for(int i = hand.size() - 2; i > 0 && count < 5; --i)
            {
                auto cur = PRIORITY_CARDS.find(hand[i].value)->second;
                auto back = PRIORITY_CARDS.find(hand[i - 1].value)->second;
                if(cur == back + 1)
                {
                    res += back;
                    ++count;
                }
                else
                {
                    res = back;
                    count = 1;
                }
            }

            if(count == 5)
            {
                return res;
            }
        }        
        return 0;
    }

    std::map<VALUES_CARDS,uint8_t> find_pair(const Hand& hand)
    {
        std::map<VALUES_CARDS,uint8_t> result;
        for(int i = 0; i < hand.size();)
        {
            auto& card = hand[i];
            auto count = std::count_if(hand.begin(),hand.end(),[card](Card &value) { return value.value == card.value; });
            result[card.value] = count;
            i += count;
        }
        return result;
    }

    void is_pair_combination(const Hand& hand)
    {
        auto pairs = find_pair(hand);
        for(auto pair = pairs.begin(); pair != pairs.end(); pair++)
        {
            auto value = pair.key();
            auto count = pair.value();
            if(count == 4)
            {
                auto max_card = *(std::max_element(pairs.begin(), pairs.end(),
                [value](std::pair(VALUES_CARDS,uint8_t) &first, std::pair(VALUES_CARDS,uint8_t) &second)
                {
                    if(first.first == value || second.first == value)
                    {
                        return false;
                    }
                    else
                    {
                        auto &first_score = PRIORITY_CARDS.find(first.first)->second;
                        auto &second_score = PRIORITY_CARDS.find(second.first)->second;
                        return first_score < second_score;
                    }
                })).first;
                uint64_t score = value * COMBINATION_MULS.find(Combination::FOUR_OF_KIND)->second + max_card;
            }
        }
    }
};

#endif // REFEREE_H
