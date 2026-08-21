/**
 * @file PlayerDriver.cpp
 * @brief Assignment 1 : Player class driver
 * @author Cuneyt YILDIRIM
 * @version 1.0
 *
 */

#include "../cards/Cards.h"
#include "../map/map.h"
#include "Player.h"
#include <iostream>
#include <vector>
using std::cout;
using std::endl;

int main() {
  cout << "\n=============================================" << endl;
  cout << "==== Player Class Functionality Testing =====" << endl;
  cout << "=============================================\n" << endl;

  // ---- Create players ----
  cout << "---- 1. Creating players ----\n" << endl;

  int armyCounter = 5;
  Player *janedoe = new Player("Jane Doe");
  Player *johnwick = new Player("John Wick");
  janedoe->setArmyPool(new int(armyCounter));
  cout << "Created players: Jane Doe and John Wick\n" << endl;

  // ---- Create territories ----
  cout << "---- 2. Creating territories ----\n" << endl;

  Territory *quebec = new Territory(1, "Quebec", 1);
  Territory *ontario = new Territory(2, "Ontario", 1);
  Territory *alberta = new Territory(3, "Alberta", 1);
  Territory *newBrunswick = new Territory(4, "New Brunswick", 1);
  Territory *newYork = new Territory(5, "New York", 1);

  cout << "Created territories: Quebec, Ontario, Alberta, New Brunswick, "
          "New York\n"
       << endl;

  // ---- Assign territories to players ----
  cout << "---- 3. Assigning territories to players ----\n" << endl;
  std::vector<Territory *> *territories1 = new std::vector<Territory *>();
  std::vector<Territory *> *territories2 = new std::vector<Territory *>();
  territories1->push_back(quebec);
  territories1->push_back(ontario);
  territories1->push_back(alberta);
  territories1->push_back(newBrunswick);
  territories1->push_back(newYork);

  janedoe->setAllTerritories(territories1);
  johnwick->setAllTerritories(territories2);
  janedoe->addTerritory(quebec);
  janedoe->addTerritory(ontario);
  janedoe->addTerritory(alberta);
  janedoe->addTerritory(newBrunswick);
  johnwick->addTerritory(newYork);

  cout << "Jane Doe owns: Quebec, Ontario, Alberta, New Brunswick" << endl;
  cout << "John Wick owns: New York\n" << endl;

  // ---- Player owns a hand of cards ----
  cout << "---- 4. Assigning hand of cards ----\n" << endl;

  Hand *janeDoeHand = new Hand();
  janeDoeHand->addCard(new Card(CardType::Bomb));
  janeDoeHand->addCard(new Card(CardType::Reinforcement));
  janeDoeHand->addCard(new Card(CardType::Diplomacy));
  janeDoeHand->addCard(new Card(CardType::Airlift));
  janeDoeHand->addCard(new Card(CardType::Blockade));
  janedoe->setHand(janeDoeHand);

  cout << "Jane Doe's hand: Bomb, Reinforcement, Diplomacy, Airlift, Blockade"
       << endl;
  cout << "Hand size: " << janedoe->getHand()->size() << "\n" << endl;
  cout << "Hand : " << *janedoe->getHand() << "\n" << endl;

  // ---- toDefend() — select territories to defend ----
  cout << "---- 5. toDefend() — select territories to defend ----\n" << endl;

  std::vector<Territory *> *defendList = janedoe->toDefend();
  cout << "Territories selected for defense: " << defendList->size() << "\n"
       << endl;

  // ---- toAttack() — select territories to attack ----
  cout << "---- 6. toAttack() — select territories to attack ----\n" << endl;

  std::vector<Territory *> *attackList = janedoe->toAttack();
  cout << "Territories selected for attack: " << attackList->size() << "\n"
       << endl;

  // ---- issueOrder() — create orders and add to order list ----
  cout << "---- 7. issueOrder() — all 6 order types ----\n" << endl;

  Order *deployOrder = new Deploy(janedoe, quebec, new int(3));
  Order *advanceOrder = new Advance(janedoe, quebec, ontario, new int(2));

  janedoe->issueOrder(Player::OrderType::EDeploy);
  janedoe->issueOrder(Player::OrderType::EAdvance);
  janedoe->issueOrder(Player::OrderType::EBomb);
  janedoe->issueOrder(Player::OrderType::EBlockade);
  janedoe->issueOrder(Player::OrderType::EAirlift);
  janedoe->issueOrder(Player::OrderType::ENegotiate);

  cout << "\nJane Doe's total orders: " << janedoe->getOrderList()->getSize()
       << "\n"
       << endl;

  cout << "---- 7.5 Order executing verification ----\n" << endl;
  cout << *deployOrder << endl;
  cout << *advanceOrder << endl;

  // ---- Stream insertion operator (operator<<) ----
  cout << "---- 8. Stream insertion operator ----\n" << endl;

  cout << *janedoe << endl;
  cout << *johnwick << endl;

  // ---- Copy constructor ----
  cout << "---- 9. Copy constructor ----\n" << endl;

  Player copiedJaneDoe(*janedoe);
  cout << "Copied Jane Doe:" << endl;
  cout << copiedJaneDoe << endl;

  // ---- Assignment operator ----
  cout << "---- 10. Assignment operator ----\n" << endl;

  Player assignedPlayer;
  assignedPlayer = *janedoe;
  cout << "Assigned Player (from Jane Doe):" << endl;
  cout << assignedPlayer << endl;

  // ---- Cleanup ----
  // Deletes all created pointers
  cout << "---- Cleanup ----" << endl;

  delete janedoe;
  delete johnwick;
  delete quebec;
  delete ontario;
  delete alberta;
  delete newBrunswick;
  delete newYork;
  delete attackList;
  delete defendList;

  cout << "\n=============================================" << endl;
  cout << "==== Player Class Testing Complete ==========" << endl;
  cout << "=============================================\n" << endl;

  return 0;
}
