#ifndef CARD_H
#define CARD_H

#include <map>
#include <vector>
#include <math.h>

enum class SUIT
{
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES
};

enum class VALUES_CARDS
{
    ACE_AS_ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEEN,
    JACK,
    QUEEN,
    KING,
    ACE
};

const std::vector<SUIT> SUITS
{
    SUIT::HEARTS,
    SUIT::DIAMONDS,
    SUIT::CLUBS,
    SUIT::SPADES
};

const std::vector<VALUES_CARDS> VALUES
{
    VALUES_CARDS::ACE_AS_ONE,
    VALUES_CARDS::TWO,
    VALUES_CARDS::THREE,
    VALUES_CARDS::FOUR,
    VALUES_CARDS::FIVE,
    VALUES_CARDS::SIX,
    VALUES_CARDS::SEVEN,
    VALUES_CARDS::EIGHT,
    VALUES_CARDS::NINE,
    VALUES_CARDS::TEEN,
    VALUES_CARDS::JACK,
    VALUES_CARDS::QUEEN,
    VALUES_CARDS::KING,
    VALUES_CARDS::ACE
};

const std::map<VALUES_CARDS,uint32_t> PRIORITY_CARDS
{
    {VALUES_CARDS::ACE_AS_ONE,  std::pow(2,0)},
    {VALUES_CARDS::TWO,         std::pow(2,1)},
    {VALUES_CARDS::THREE,       std::pow(2,2)},
    {VALUES_CARDS::FOUR,        std::pow(2,3)},
    {VALUES_CARDS::FIVE,        std::pow(2,4)},
    {VALUES_CARDS::SIX,         std::pow(2,5)},
    {VALUES_CARDS::SEVEN,       std::pow(2,6)},
    {VALUES_CARDS::EIGHT,       std::pow(2,7)},
    {VALUES_CARDS::NINE,        std::pow(2,8)},
    {VALUES_CARDS::TEEN,        std::pow(2,9)},
    {VALUES_CARDS::JACK,        std::pow(2,10)},
    {VALUES_CARDS::QUEEN,       std::pow(2,11)},
    {VALUES_CARDS::KING,        std::pow(2,12)},
    {VALUES_CARDS::ACE,         std::pow(2,13)}
};

struct Card
{
    VALUES_CARDS value;
    SUIT suit;

    /*Card()
    {
        value = VALUES_CARDS::ACE_AS_ONE;
        suit = SUIT::CLUBS;
    }

    Card(const Card& item)
    {
        value = item.value;
        suit = item.suit;
    }

    Card(Card&& item)
    {
        value = item.value;
        suit = item.suit;
    }

    Card& operator=(const Card& item)
    {
       if(this != &item)
       {
           value = item.value;
           suit = item.suit;
       }
       return *this;
    }

    Card& operator=(Card&& item)
    {
       if(this != &item)
       {
           value = item.value;
           suit = item.suit;
       }
       return *this;
    }

    virtual ~Card(){}*/
};

using Hand = std::vector<Card>;

#endif // CARD_H
