#ifndef CARDS_H
#define CARDS_H

#include "../Orders/Orders.h"
#include <iostream>
#include <vector>
using namespace std;

class Player;
class Hand;
class Deck;

// List of card types
enum class CardType {
    Bomb, 
    Reinforcement, 
    Blockade, 
    Airlift, 
    Diplomacy
};

class Card {
private:
    CardType* type;  // Pointer to card type
public:
    Card(CardType); 
    Card(const Card& other);
    Card& operator=(const Card& other); // Declares assignment operator to create deep copy 
    ~Card();

    CardType getType() const;

    void play(Hand* hand, Deck* deck, Player* player); 

    friend ostream& operator<<(ostream& out, const Card& card);  // Stream insertion operator
};

class Deck {
private:
    vector<Card*>* cards;
public:
    Deck();  
    Deck(const Deck& other);
    Deck& operator=(const Deck& other); 
    ~Deck();

    int size() const;

    Card* draw(Hand* hand);
    void addCard(Card* card);

    friend ostream& operator<<(ostream& out, const Deck& deck);
};

class Hand {
private:
    vector<Card*>* cards;
public:
    Hand();
    Hand(const Hand& other);
    Hand& operator=(const Hand& other); 
    ~Hand();

    int size() const;

    void addCard(Card* card);
    bool isEmpty() const;
    Card* getCardAt(int index);
    void removeCard(Card* card);

    friend ostream& operator<<(ostream& out, const Hand& hand);
};

#endif
