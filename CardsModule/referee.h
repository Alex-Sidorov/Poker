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

    using Flushes = std::vector<std::pair<Hand, uint64_t>>;

public:
    Referee(){}
    virtual ~Referee(){}

    virtual std::map<T,Combination> refereeing(const std::map<T,Hand> &hands)
    {
        std::map<T,std::future<std::pair<Combination,uint64_t>>> tasks;
        for (auto it = hands.begin(); it != hands.end(); ++it)
        {
            auto& player = it->first;
            tasks[player] = std::async(&Referee::get_score,this,std::cref(it->second));
        }

        uint64_t temp = 0;

        std::map<T,Combination> winner;
        for (auto it = tasks.begin(); it != tasks.end(); ++it)
        {
            auto& player = it->first;
            std::pair<Combination,uint64_t> res = it->second.get();

            auto &combination = res.first;
            auto &score = res.second;

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

    std::pair<Combination,uint64_t> get_score(const Hand& hand)
    {
        if(hand.size() < 5)
        {
            return std::make_pair(Combination::KICKER,0);
        }

        std::pair<Combination,uint64_t> res{Combination::KICKER,0};
        auto flushes = find_flushes(hand);
        if(flushes.size())
        {
            res = get_max_res_flush(flushes);
            if(res.first == Combination::STRAIGHT_FLUSH)
            {
                return res;
            }
        }
        else
        {
            auto score = is_straight(hand);
            if(score)
            {
                res.first = Combination::STRAIGHT;
                res.second = score;
            }
        }

        auto res_pair = is_pair_combination(hand);
        if(res.second > res_pair.second)
        {
            return res;
        }
        else if(res.second < res_pair.second)
        {
            return res_pair;
        }

        uint64_t score = 0;
        for(size_t index = hand.size() - 1, count = 0; count < 5; --index, ++count)
        {
            score += PRIORITY_CARDS.find(hand[index].value)->second;
        }
        return {Combination::KICKER, score};
    }

    std::pair<Combination, uint64_t> get_max_res_flush(Flushes& flushes)
    {
        std::pair<Combination, uint64_t> res{Combination::FLUSH,0};
        for(auto& flush : flushes)
        {
            auto& hand = flush.first;
            auto& score = flush.second;
            if(res.second < score)
            {
                res.second = score;
            }

            auto score_straight_flush = is_straight(hand);
            if(score_straight_flush)
            {
                score_straight_flush *= COMBINATION_MULS.find(Combination::STRAIGHT_FLUSH)->second;
                if(res.second < score_straight_flush)
                {
                    res.first = Combination::STRAIGHT_FLUSH;
                    res.second = score_straight_flush;
                }
            }
        }
        return res;
    }

    Flushes find_flushes(const Hand& origin_hand)
    {
        Hand hand = origin_hand;
        sort_by_value(hand);

        Flushes res;
        for(auto& suit : SUITS)
        {
            auto count_cards = std::count_if(hand.begin(),hand.end(),[&](const Card& value) { return value.suit == suit; });
            if(count_cards >= 5)
            {
                Hand all_cards(count_cards);
                std::copy_if(hand.begin(),hand.end(),all_cards.begin(),[suit](const Card& card)
                {
                    return card.suit == suit;
                });
                for(size_t i = 0; i <= all_cards.size() - 5; ++i)
                {
                    Hand cards(5);
                    std::copy_n(all_cards.begin() + i, 5, cards.begin());
                    auto score = count_score(cards) * COMBINATION_MULS.find(Combination::FLUSH)->second;
                    res.push_back({std::move(cards), score});
                }
            }
        }
        return res;
    }

    Hand exec_for_check_straight(const Hand& origin_hand)
    {
        Hand hand = origin_hand;
        remove_pairs(hand);
        add_ace_as_one(hand);
        sort_by_value(hand);
        return hand;
    }

    uint64_t is_straight(const Hand& origin_hand)
    {
        Hand hand = origin_hand;
        remove_pairs(hand);
        add_ace_as_one(hand);
        sort_by_value(hand);
        if(hand.size())
        {
            int count = 1;
            uint64_t res = PRIORITY_CARDS.find(hand.back().value)->second;
            for(size_t i = hand.size() - 1; i > 0 && count < 5; --i)
            {
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
                return res * COMBINATION_MULS.find(Combination::STRAIGHT)->second;
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
        if(count <= hand.size() - exceptions.size())
        {
            std::vector<VALUES_CARDS> temp;
            for(int index = hand.size() - 1; index >= 0 && count != 0; --index)
            {
                if(std::find(exceptions.begin(), exceptions.end(),hand[index].value) == exceptions.end()
                        && std::find(temp.begin(), temp.end(),hand[index].value) == temp.end())
                {
                    res += PRIORITY_CARDS.find(hand[index].value)->second;
                    temp.push_back(hand[index].value);
                    --count;
                }
            }
        }
        return res;
    }

    uint64_t count_score(const Hand& hand)
    {
        uint64_t res = 0;
        for(auto& item : hand)
        {
            auto &score = PRIORITY_CARDS.find(item.value)->second;
            res += score;
        }
        return res;
    }

    void remove_pairs(Hand &hand)const
    {
        auto it = hand.begin();
        if(it == hand.end())
        {
            return;
        }
        do
        {
            auto temp = *it;
            auto count = std::count_if(hand.begin(), hand.end(),[&temp](Card &card)
            {
                return card.value == temp.value;
            });
            if(count > 1)
            {
                hand.erase(it);
                it = hand.begin();
            }
            else
            {
                ++it;
            }
        }while(it != hand.end());
    }

    void sort_by_value(Hand& hand)
    {
        std::sort(hand.begin(),hand.end(),[&](const Card &first, const Card &second)
        {
            auto &first_score = PRIORITY_CARDS.find(first.value)->second;
            auto &second_score = PRIORITY_CARDS.find(second.value)->second;
            return first_score < second_score;
        });
    }

    void sort_by_suit(Hand& hand)
    {
        std::sort(hand.begin(),hand.end(),[&](const Card &first, const Card &second)
        {
            return first.suit < second.suit;
        });
    }

    void add_ace_as_one(Hand& hand)
    {
        auto cur = std::find_if(hand.begin(),hand.end(),[](Card& card)
        {
            return card.value == VALUES_CARDS::ACE;
        });
        while(cur != hand.end())
        {
            cur = hand.insert(cur + 1,{VALUES_CARDS::ACE_AS_ONE, cur->suit});
            cur = std::find_if(cur,hand.end(),[](Card& card)
            {
                return card.value == VALUES_CARDS::ACE;
            });
        }

    }
};

#endif // REFEREE_H
