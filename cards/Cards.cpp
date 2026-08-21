#include "Cards.h"
#include "../Player/Player.h"
using namespace std;

//-----Card implementation-----//

// Default constructor
Card::Card(CardType t) {
    type = new CardType(t);
}

// Copy constructor
Card::Card(const Card& other) {
    type = new CardType(*other.type);
}

// Performs deep copy of Card
Card& Card::operator=(const Card& other) {
    if (this != &other) {
        delete type;  // Deallocate old data
        type = new CardType(*other.type);  // Allocate new data
    }
    return *this;
}

// Destructor
Card::~Card() {
    delete type;
    type = nullptr;
}

// Getter
CardType Card::getType() const {
    return *type;
}

// Play method
void Card::play(Hand* hand, Deck* deck, Player* player) {
    Order* order = nullptr;
    cout << "Playing " << *this << endl;

    // Create order based on card type
    switch(*type) {
        case CardType::Bomb:
            order = new Bomb();
            break;
        case CardType::Reinforcement: // Adds 5 armies to player's army pool, doesn't create an order
            player->setArmyPool(new int(*player->getArmyPool() + 5));
            cout << "   - Reinforcement: 5 armies added to pool" << endl;
            break;
        case CardType::Blockade:
            order = new Blockade();
            break;
        case CardType::Airlift:
            order = new Airlift();
            break;
        case CardType::Diplomacy:
            order = new Negotiate();
            break;
    }

    if (order != nullptr) {
        // Add order to order list
        cout << "   - Order created and added to player's orders list" << endl;
        player->getOrderList()->addOrder(order);
    }

    // Put card back in deck
    hand->removeCard(this);
    deck->addCard(this);
}

// Stream insertion operator
ostream& operator<<(ostream& out, const Card& card) {
    switch(*card.type) {
        case CardType::Bomb: 
            out << "Bomb"; 
            break;
        case CardType::Reinforcement:
            out << "Reinforcement"; 
            break;   
        case CardType::Blockade: 
             out << "Blockade"; 
            break;   
        case CardType::Airlift: 
             out << "Airlift"; 
            break;                    
        case CardType::Diplomacy: 
             out << "Diplomacy"; 
            break;   
    }
    return out;
}


//-----Deck implementation-----//

Deck::Deck() {
    cards = new vector<Card*>();
}

Deck::Deck(const Deck& other) {
    cards = new vector<Card*>();
    for (int i=0; i<other.cards->size(); i++) {
        cards->push_back(new Card(*other.cards->at(i)));
    }
}

Deck& Deck::operator=(const Deck& other) { 
    if (this != &other) {
        for (int i=0; i < cards->size(); i++) {
            delete cards->at(i);
        }
        delete cards;

        cards = new vector<Card*>();
        for (int i=0; i<other.cards->size(); i++) {
            cards->push_back(new Card(*other.cards->at(i)));
        }
    }
    return *this;
}

Deck::~Deck() {
    for (int i=0; i<cards->size(); i++) {
        delete cards->at(i);
    }
    delete cards;
}

// Returns size of deck
int Deck::size() const{
    return cards->size();
}

// Draw method
Card* Deck::draw(Hand* hand) {
    if (cards->empty()) return nullptr;

    int randomIndex = rand() % cards->size(); 
    Card* drawn = cards->at(randomIndex); // Pick random card from deck
    
    cards->erase(cards->begin() + randomIndex); // Remove card from deck
    hand->addCard(drawn); // Call addCard method
    return drawn;
}

// Adds card to deck
void Deck::addCard(Card* card) {
    (*cards).push_back(card);
}

// Displays each card in deck
ostream& operator<<(ostream& out, const Deck& deck) {
    for (int i=0; i<deck.cards->size(); i++) {
            out << "- " << *(deck.cards->at(i)) << endl;
    }
    return out;
}

//-----Hand implementation-----//

Hand::Hand() {
    cards = new vector<Card*>();
}

Hand::Hand(const Hand& other) {
    cards = new vector<Card*>();
    for (int i=0; i<other.cards->size(); i++) {
        cards->push_back(other.cards->at(i));
    }
}

Hand& Hand::operator=(const Hand& other) {
    if (this != &other) {
        delete cards;

        cards = new vector<Card*>();
        for (int i=0; i<other.cards->size(); i++) {
            cards->push_back(other.cards->at(i));
        }
    }
    return *this;
}

Hand::~Hand() {
    delete cards;
}

// Returns size of hand
int Hand::size() const {
    return cards->size();
}

bool Hand::isEmpty() const {
    return cards->empty();
}

// Returns pointer to card at given index
Card* Hand::getCardAt(int index) {
    return cards->at(index);
}

// Adds card to hand
void Hand::addCard(Card* card) {
    cards->push_back(card);
}

// Removes card from hand
void Hand::removeCard(Card* card) {
    for (int i=0; i<cards->size(); i++) {
        if (cards->at(i) == card) {
            cards->erase(cards->begin() + i);
            return;
        }
    }
}

// Displays each card in hand
ostream& operator<<(ostream& out, const Hand& hand) {
    for (int i=0; i < hand.cards->size(); i++) {
            out << "- "  << *(hand.cards->at(i)) << endl;
    }
    return out;
}