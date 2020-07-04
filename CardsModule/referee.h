#ifndef REFEREE_H
#define REFEREE_H

#include "abstractreferee.h"
#include "combinations.h"

#include <algorithm>
#include <future>
#include <vector>
#include <map>

template<class T>
class Referee : public AbstractReferee<T>
{

public:
    Referee(){}
    virtual ~Referee(){}


    virtual std::map<T,Combination> refereeing(const std::map<T,Hand&> &hands)
    {
        //std::map<T,std::future<std::pair<Combination,uint64_t>>> scores;
        std::map<T,std::pair<Combination,uint64_t>> scores;
        for (auto it = hands.begin(); it != hands.end(); ++it)
        {
            auto player = it->first;
            auto &hand =   it->second;

            scores[player] = get_score(hand);
            //auto future = std::async( [&] { this->get_score(std::cref(hand)); });
            //scores.emplace(player, future);
        }

        uint64_t temp = 0;

        std::map<T,Combination> winner;
        for (auto it = scores.begin(); it != scores.end(); ++it)
        {
            auto& player = it->first;
            //auto res =  it->second.get();
            auto &res = it->second;
            auto &score = res.second;
            auto &combination = res.first;

            if(temp == score)
            {
                winner[player] = combination;
            }
            else if(temp < score)
            {
                winner.clear();
                winner[player] = combination;
                temp = score;
            }
        }
        return winner;
    }

private:

    std::pair<Combination,uint64_t> get_score(Hand& hand)
    {
        if(hand.size() < 5)
        {
            return std::make_pair(Combination::KICKER,0);
        }
        std::sort(hand.begin(),hand.end(),[&](const Card &first, const Card &second)
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
            std::copy(hand.begin() + begin_flush, hand.begin() + begin_flush + 5, temp.begin());

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


        auto res_pair = is_pair_combination(hand);
        if(res_pair.first != Combination::KICKER)
        {
            return res_pair;
        }

        uint64_t res = 0;
        for(size_t index = hand.size() - 1, count = 0; count < 5; --index, ++count)
        {
            res += PRIORITY_CARDS.find(hand[index].value)->second;
        }
        return {Combination::KICKER, res};
    }

    std::pair<uint64_t,int> is_flush(const Hand& hand)
    {        
        for(auto& suit : SUITS)
        {
            auto count_cards = std::count_if(hand.begin(),hand.end(),[&](Card value) { return value.suit == suit; });
            if(count_cards >= 5)
            {
                uint64_t res = 0;
                auto index = hand.size();
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
                return {res, index+1};
            }
        }
        return {0,0};
    }

    uint64_t is_straight(Hand hand)
    {        
        if(hand.size())
        {
            Card ace_as_one{VALUES_CARDS::ACE,SUIT::CLUBS};
            int count = 1;
            uint64_t res = PRIORITY_CARDS.find(hand.back().value)->second;
            for(size_t i = hand.size() - 1; i > 0 && count < 5; --i)
            {
                if(hand[i].value == VALUES_CARDS::ACE)
                {
                    ace_as_one = {VALUES_CARDS::ACE_AS_ONE,hand[i].suit};
                }
                if(hand[i].value == VALUES_CARDS::FIVE && count == 5 && ace_as_one.value == VALUES_CARDS::ACE_AS_ONE)
                {
                    hand.insert(hand.begin(),ace_as_one);
                }
                auto cur = PRIORITY_CARDS.find(hand[i].value)->second;
                auto back = PRIORITY_CARDS.find(hand[i - 1].value)->second;
                if(cur == 2*back)
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
        for(size_t i = 0; i < hand.size();)
        {
            auto& card = hand[i];

            auto count = std::count_if(hand.begin(),hand.end(),
            [card](Card check_card)
            {
                return check_card.value == card.value;
            });

            result[card.value] = count;
            i += count;
        }
        return result;
    }

    std::pair<Combination,uint64_t> is_pair_combination(const Hand& hand)
    {
        VALUES_CARDS first_pair_combination{VALUES_CARDS::ACE_AS_ONE};
        VALUES_CARDS second_pair_combination{VALUES_CARDS::ACE_AS_ONE};
        VALUES_CARDS three_of_kind{VALUES_CARDS::ACE_AS_ONE};

        auto pairs = find_pair(hand);
        for(auto pair = pairs.begin(); pair != pairs.end(); pair++)
        {
            auto &value = pair->first;
            auto &count = pair->second;
            if(count == 4)
            {
                auto max_card = find_score_any_max_card(hand,1,{value});
                auto &priority_card = PRIORITY_CARDS.find(value)->second;
                uint64_t score = priority_card * COMBINATION_MULS.find(Combination::FOUR_OF_KIND)->second + max_card;
                return std::make_pair(Combination::FOUR_OF_KIND,score);
            }
            else if(count == 3)
            {
                auto &cur_three = PRIORITY_CARDS.find(value)->second;
                auto &back_three = PRIORITY_CARDS.find(three_of_kind)->second;
                if(cur_three > back_three)
                {
                    three_of_kind = value;
                }
            }
            else if(count == 2)
            {
                auto &cur_pair = PRIORITY_CARDS.find(value)->second;
                auto &back_pair = PRIORITY_CARDS.find(first_pair_combination)->second;
                if(cur_pair > back_pair)
                {
                    second_pair_combination = first_pair_combination;
                    first_pair_combination = value;
                }
            }
        }

        if(first_pair_combination != VALUES_CARDS::ACE_AS_ONE && three_of_kind != VALUES_CARDS::ACE_AS_ONE)
        {
            auto &priority_card_three_of_kind = PRIORITY_CARDS.find(three_of_kind)->second;
            auto &mul_three_of_kind = COMBINATION_MULS.find(Combination::THREE_OF_KIND)->second;
            auto &priority_pair_card = PRIORITY_CARDS.find(first_pair_combination)->second;

            uint64_t score = priority_card_three_of_kind * mul_three_of_kind + priority_pair_card;
            return std::make_pair(Combination::FULL_HOUSE,score);
        }
        else if(three_of_kind != VALUES_CARDS::ACE_AS_ONE)
        {
            auto &priority_card_three_of_kind = PRIORITY_CARDS.find(three_of_kind)->second;
            auto &mul_three_of_kind = COMBINATION_MULS.find(Combination::THREE_OF_KIND)->second;

            auto other_cards_score = find_score_any_max_card(hand,2,{three_of_kind});

            uint64_t score = priority_card_three_of_kind * mul_three_of_kind + other_cards_score;
            return std::make_pair(Combination::THREE_OF_KIND,score);
        }
        else if(first_pair_combination != VALUES_CARDS::ACE_AS_ONE && second_pair_combination != VALUES_CARDS::ACE_AS_ONE)
        {
            auto &priority_first_pair_card = PRIORITY_CARDS.find(first_pair_combination)->second;
            auto &priority_second_pair_card = PRIORITY_CARDS.find(second_pair_combination)->second;
            auto &mul_two_pair = COMBINATION_MULS.find(Combination::TWO_PAIR)->second;

            auto other_cards_score = find_score_any_max_card(hand,1,{first_pair_combination, second_pair_combination});

            uint64_t score = (priority_first_pair_card + priority_second_pair_card) * mul_two_pair + other_cards_score;
            return std::make_pair(Combination::TWO_PAIR,score);
        }
        else if(first_pair_combination != VALUES_CARDS::ACE_AS_ONE)
        {
            auto &priority_pair_card = PRIORITY_CARDS.find(first_pair_combination)->second;
            auto &mul_one_pair = COMBINATION_MULS.find(Combination::ONE_PAIR)->second;

            auto other_cards_score = find_score_any_max_card(hand,4,{first_pair_combination});

            uint64_t score = priority_pair_card * mul_one_pair + other_cards_score;
            return std::make_pair(Combination::ONE_PAIR,score);
        }
        return std::make_pair(Combination::KICKER,0);
    }

    uint64_t find_score_any_max_card(const Hand &hand, size_t count, const std::vector<VALUES_CARDS> &exceptions = {})
    {
        uint64_t res = 0;
        if(count <= hand.size())
        {
            std::vector<VALUES_CARDS> temp;
            for(size_t i = 0; i < count; i++)
            {
                auto& card = *(std::max_element(hand.begin(), hand.end(),
                [&](Card first, Card second)
                {
                    auto first_is_exception = std::find(exceptions.begin(), exceptions.end(),first.value) != exceptions.end();
                    if(first_is_exception)
                    {
                        return false;
                    }
                    auto second_is_exception = std::find(exceptions.begin(), exceptions.end(),second.value) != exceptions.end();
                    if(second_is_exception)
                    {
                        return false;
                    }
                    auto first_is_used = std::find(temp.begin(), temp.end(),first.value) != exceptions.end();
                    if(first_is_used)
                    {
                        return false;
                    }
                    auto second_is_used = std::find(temp.begin(), temp.end(),second.value) != exceptions.end();
                    if(second_is_used)
                    {
                        return false;
                    }
                    else
                    {
                        auto &first_score = PRIORITY_CARDS.find(first.value)->second;
                        auto &second_score = PRIORITY_CARDS.find(second.value)->second;
                        return first_score < second_score;
                    }
                }));
                //temp.push_back(card.value);
                res += PRIORITY_CARDS.find(card.value)->second;
            }
        }
        return res;
    }
};

#endif // REFEREE_H
