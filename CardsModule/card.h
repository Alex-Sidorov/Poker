#ifndef CARD_H
#define CARD_H

#include <map>
#include <vector>

enum class SUIT
{
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES
};

enum class VALUES_CARDS
{
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
    {VALUES_CARDS::TWO,1},
    {VALUES_CARDS::THREE,2},
    {VALUES_CARDS::FOUR,3},
    {VALUES_CARDS::FIVE,4},
    {VALUES_CARDS::SIX,5},
    {VALUES_CARDS::SEVEN,6},
    {VALUES_CARDS::EIGHT,7},
    {VALUES_CARDS::NINE,8},
    {VALUES_CARDS::TEEN,9},
    {VALUES_CARDS::JACK,10},
    {VALUES_CARDS::QUEEN,11},
    {VALUES_CARDS::KING,12},
    {VALUES_CARDS::ACE,13}
};

struct Card
{
    VALUES_CARDS value;
    SUIT suit;
};

using Hand = std::vector<Card>;

#endif // CARD_H
