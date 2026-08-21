/*
Assignment 1, Part 4: Cards deck/hand
- Creates a deck with 15 cards of 5 possible types (Bomb, Reinforcement, Blockade, Airlift, Diplomacy)
- Draws 5 cards randomly from the deck into hand
- Plays all the cards in hand, returns them to the deck
*/

#include "Cards.h"
#include "../Player/Player.h"
#include <iostream>
// For randomizing card drawing
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    srand(time(0)); //Random seed to get different results each run

    // Create deck object
    cout << endl << "----- PART 1: Creating Deck -----" << endl;
    cout << endl;
    cout << "Creating deck..." << endl;
    Deck* deck = new Deck();

    // Add all card types to deck (3 of each for now)
    cout << "Adding cards to deck..." << endl;
    for (int i = 0; i < 3; i++) {
        deck->addCard(new Card(CardType::Bomb));    
        deck->addCard(new Card(CardType::Reinforcement));    
        deck->addCard(new Card(CardType::Blockade));    
        deck->addCard(new Card(CardType::Airlift));    
        deck->addCard(new Card(CardType::Diplomacy));  
    }

    // Display deck
    cout << endl;
    cout<< "Deck (" << deck->size()<< " cards):" << endl;
    cout << *deck << endl;

    // Create hand object
    cout << "----- PART 2: Drawing Cards -----" << endl;
    cout << endl;
    cout << "Creating hand..." << endl;
    cout << "Randomly drawing cards from deck..." << endl; 


    Player* player = new Player("Jane Doe");
    Hand* hand = player->getHand();

    cout << endl;

    // Draw cards from deck, add them to hand
    for (int i = 0; i < 5; i++) {
        Card* drawn = deck->draw(hand);
        cout << "Drew " << *drawn << endl;
    }
    
    // Display hand
    cout << endl;
    cout<< "Hand (" << hand->size()<< " cards):" << endl;
    cout << *hand << endl;

    // Display deck
    cout << "Displaying deck after drawing cards..." << endl;
    cout << endl;
    cout << "Deck (" << deck->size()<< " cards):" << endl;
    cout << *deck << endl;

    // Play cards in hand
    cout << "----- PART 3: Playing Cards -----" << endl;
    cout << endl;

    while (!hand->isEmpty()) {
        Card* cardToPlay = hand->getCardAt(0);
        cardToPlay->play(hand,deck, player);
    }

    // Display player's orders list
    cout << endl;
    cout << "Player's orders after playing cards:" << endl;
    cout << *player->getOrderList() << endl;

    // Display deck
    cout << endl;
    cout << "Displaying deck after playing cards in hand..." << endl;
    cout << endl;
    cout << "Deck (" << deck->size()<< " cards):" << endl;
    cout << *deck << endl;

    // Display hand again
    cout << "Displaying hand after playing cards in hand..." << endl;
    cout << endl;
    cout<< "Hand (" << hand->size()<< " cards):" << endl;
    cout << *hand << endl;

    delete player;
    delete deck;
    return 0;
}