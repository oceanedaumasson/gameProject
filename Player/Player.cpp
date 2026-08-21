/**
 * @file Player.cpp
 * @brief Assignment 1 : Player class
 * @author Cuneyt YILDIRIM
 * @version 1.0
 *
 * This file contains for the Player class functionality and creating all need
 * it methods and attributes
 */
#include "Player.h"
#include "../Orders/Orders.h"
#include "../PlayerStrategy/PlayerStrategies.h"
#include "../cards/Cards.h"
#include "../map/map.h"
#include "algorithm"
#include "iostream"
#include <cstddef>
#include <ostream>
#include <string>
#include <vector>
using namespace std;

/**
 * @brief Default constructor
 * Player's name is created as 'Default Player'
 */
Player::Player() {
  playerName = new std::string("Default Player");
  this->territories = new std::vector<Territory *>();
  this->setHand(new Hand());
  this->orderslist = new OrdersList();
  this->allTerritories = new std::vector<Territory *>();
  this->armyPool = new int(0);
  this->conqueredThisTurn = new bool(false);
  this->negotiatedPlayers = new std::vector<Player *>();
  this->playerStrategy = nullptr; // change to human strategy
}

Player::Player(PlayerStrategy *strategy) {
  playerName = new std::string("Default Player");
  this->territories = new std::vector<Territory *>();
  this->setHand(new Hand());
  this->orderslist = new OrdersList();
  this->allTerritories = new std::vector<Territory *>();
  this->armyPool = new int(0);
  this->conqueredThisTurn = new bool(false);
  this->negotiatedPlayers = new std::vector<Player *>();
  this->playerStrategy = strategy;
}

/**
 * @brief parametirezed constructor
 * @param name the Player name.
 */
Player::Player(const std::string &name) {
  this->playerName = new std::string(name);
  this->territories = new std::vector<Territory *>();
  this->setHand(new Hand());
  this->orderslist = new OrdersList();
  this->allTerritories = new std::vector<Territory *>();
  this->armyPool = new int(0);
  this->conqueredThisTurn = new bool(false);
  this->negotiatedPlayers = new std::vector<Player *>();
  this->playerStrategy = nullptr; // change to human strategy
}

Player::Player(const string &name, PlayerStrategy *strategy) {
  this->playerName = new std::string(name);
  this->territories = new std::vector<Territory *>();
  this->setHand(new Hand());
  this->orderslist = new OrdersList();
  this->allTerritories = new std::vector<Territory *>();
  this->armyPool = new int(0);
  this->conqueredThisTurn = new bool(false);
  this->negotiatedPlayers = new std::vector<Player *>();
  this->playerStrategy = strategy;
}

/**
 * @brief copy constructor
 * @param oldPlayer the Player's reference .
 */
Player::Player(const Player &prevPlayer) {

  // copy strategy
  this->playerStrategy = prevPlayer.playerStrategy;

  // Deep copy for playerName
  this->playerName = new std::string(*prevPlayer.playerName);

  // Deep copy for territories
  this->territories = new std::vector<Territory *>();
  for (Territory *ter : *prevPlayer.territories) {
    this->territories->push_back(ter);
  }

  // Deep copy for playerHand
  this->playerHand = new Hand(*prevPlayer.playerHand);

  // Deep copy for orderslist
  this->orderslist = new OrdersList(*prevPlayer.orderslist);

  // Copy primitive types
  this->armyPool = new int(*prevPlayer.armyPool);
  this->allTerritories = new std::vector<Territory *>();
  for (Territory *ter : *prevPlayer.allTerritories) {
    allTerritories->push_back(ter);
  }
  this->conqueredThisTurn = new bool(*prevPlayer.conqueredThisTurn);
  this->negotiatedPlayers =
      new std::vector<Player *>(*prevPlayer.negotiatedPlayers);
}

/**
 * Assignment constructor
 * @param oldPlayer copying previous Player object.
 */
Player &Player::operator=(const Player &prevPlayer) {
  if (this != &prevPlayer) {
    // Clean up existing resources
    delete playerName;

    // Cleanup for armyPool:
    delete armyPool;

    // Cleanup for territories:
    delete territories;

    // Cleanup for playerHand:
    delete playerHand;

    // Cleanup for orderslist:
    delete orderslist;

    // Cleanup for attackTerritories:
    delete allTerritories;

    playerStrategy = prevPlayer.playerStrategy;

    // deep copy from prevPlayer
    playerName = new std::string(prevPlayer.getPlayerName());

    // Deep copy for territories:
    territories = new std::vector<Territory *>();
    for (Territory *ter : *prevPlayer.getTerritories()) {
      territories->push_back(new Territory(*ter));
    }

    // Deep copy for playerHand:
    playerHand = new Hand(*prevPlayer.getHand());

    // Deep copy for orderslist:
    orderslist = new OrdersList(*prevPlayer.getOrderList());

    armyPool = new int(*prevPlayer.armyPool);

    allTerritories = new std::vector<Territory *>();
    for (Territory *ter : *prevPlayer.allTerritories) {
      allTerritories->push_back(ter);
    }
    this->conqueredThisTurn = new bool(*prevPlayer.conqueredThisTurn);
    this->negotiatedPlayers =
        new std::vector<Player *>(*prevPlayer.negotiatedPlayers);
  }
  return *this;
}

// Deleting assigned pointers
Player::~Player() {
  delete playerName;
  delete territories;
  delete playerHand;
  delete orderslist;
  delete allTerritories;
  delete armyPool;
  delete negotiatedPlayers;
  delete conqueredThisTurn;
  delete playerStrategy;
}

/**
 * Assignment constructor
 * @param oldPlayer copying previous Player object.
 */
std::ostream &operator<<(std::ostream &out, const Player &player) {
  out << "Player: " << player.getPlayerName() << std::endl;
  out << "\t Deployed Army Pool: " << *player.getArmyPool() << std::endl;
  out << "\t Territoris owned total: " << player.getTerritories()->size()
      << std::endl;
  out << "\t\t Owned Territory Name: ";
  for (Territory *t : *player.getTerritories()) {
    out << t->getName() << ", ";
  }
  out << std::endl;
  out << "\t Hand: " << *player.getHand() << std::endl;
  out << "\t Orders: " << *player.getOrderList() << std::endl;
  return out;
}

// getters
std::string Player::getPlayerName() const { return *playerName; }

Hand *Player::getHand() const { return playerHand; }

OrdersList *Player::getOrderList() const { return orderslist; }

int *Player::getArmyPool() const { return this->armyPool; }

std::vector<Territory *> *Player::getAllTerritories() const {
  return this->allTerritories;
}

std::vector<Player *> *Player::getNegotiatedPlayers() const {
  return this->negotiatedPlayers;
}

void Player::addNegotiatedPlayer(Player *p) { negotiatedPlayers->push_back(p); }
void Player::clearNegotiations() { negotiatedPlayers->clear(); }

void Player::setConcuredThisTurn(bool val) { *conqueredThisTurn = val; }
bool Player::hasConcuredThisTurn() const { return *conqueredThisTurn; }

bool Player::isNegotiatedWith(Player *p) const {
  for (Player *pl : *getNegotiatedPlayers()) {
    if (pl == p) {
      return true;
    }
  }
  return false;
}

/**
 * @brief Adding a new territory into the territories list
 * @param the pointer of the Territory object;
 */
void Player::addTerritory(Territory *territory) {
  if (territory != nullptr) {
    territories->push_back(territory);
  }
}

/**
 * @brief Removes an existing territory from the territories list
 * @param the pointer of the Territory object;
 */
void Player::removeTerritory(Territory *territory) {
  if (territory != nullptr) {
    std::vector<Territory *>::iterator ter =
        std::find(territories->begin(), territories->end(), territory);
    if (ter != territories->end()) {
      territories->erase(ter);
    }
  }
}

// Helper function to convert OrderType to string
std::string Player::enumToString(Player::OrderType type) const {
  switch (type) {
  case Player::OrderType::EBomb:
    return "bomb";
  case Player::OrderType::EDeploy:
    return "deploy";
  case Player::OrderType::EBlockade:
    return "blockade";
  case Player::OrderType::EAirlift:
    return "airlift";
  case Player::OrderType::ENegotiate:
    return "negotiate";
  case Player::OrderType::EAdvance:
    return "advance";
  default:
    return "unknown";
  }
}

/**
 * @brief creates new Order based on the provided enum object.
 * @param OrderType enum class to identify what is the order's type.
 * @return Order object
 */
// WARN: enum card names are conflict with the class names
Order *Player::createOrder(Player::OrderType type) const {
  if (type == OrderType::EDeploy)
    return new Deploy();
  if (type == OrderType::EAdvance)
    return new Advance();
  if (type == OrderType::EBomb)
    return new Bomb();
  if (type == OrderType::EBlockade)
    return new Blockade();
  if (type == OrderType::EAirlift)
    return new Airlift();
  if (type == OrderType::ENegotiate)
    return new Negotiate();

  std::cout << "Invalid order type: " << enumToString(type) << std::endl;
  return nullptr;
}

void Player::setPlayerStrategy(PlayerStrategy *strategy) {
  this->playerStrategy = strategy;
}

PlayerStrategy *Player::getPlayerStrategy() const { return playerStrategy; }

/**
 * @brief Issues an order for the player
 * If the player still has armies in their reinforcement pool, they must issue
 * deploy orders first Once all armies are deployed, the player can issue
 * advance orders to defend or attack, or play cards Called repeatedly by the
 * game engine in round-robin fashion until player signals they are done by
 * returning false
 * @param defend List of territories to defend in priority order (from
 * toDefend())
 * @param attack List of territories to attack in priority order (from
 * toAttack())
 * @param committedArmies Count of armies already committed to deploy orders
 * @param gameDeck the Game deck
 * @param players List of all players in the game
 * @return bool True if an order was issued, false if player is done */
bool Player::issueOrder(vector<Territory *> *defend,
                        vector<Territory *> *attack, int &committedArmies,
                        Deck *gameDeck, vector<Player *> *players) {
  if (playerStrategy == nullptr)
    return false;

  return playerStrategy->issueOrder(defend, attack, committedArmies, gameDeck,
                                    players);
}

std::vector<Territory *> *Player::getTerritories() const { return territories; }

// Checks the owner
bool Player::isTerritoryOwned(Territory *territory) const {
  return std::find(territories->begin(), territories->end(), territory) !=
         territories->end();
}

// Setter
void Player::setPlayerName(const std::string &name) {
  *(this->playerName) = name;
}

void Player::setArmyPool(int *armyPool) { this->armyPool = armyPool; }

void Player::increaseArmyPool() {
  if (*armyPool < 6) {
    *armyPool += 1;
  } else {
    cout << "You already deployed all armies"; // Fixed: << instead of <
  }
}

void Player::setHand(Hand *hand) { this->playerHand = hand; }

void Player::setAllTerritories(std::vector<Territory *> *territories) {
  this->allTerritories = territories;
}

/**
 * @brief Returns an arbitrary list of territories that the player can attack.
 *        This method currently allows the player to select territories from a
 *        predefined list of attackable territories, excluding those already
 *        owned by the player.
 * @return A vector of Territory pointers that the player has chosen to attack.
 */
std::vector<Territory *> *Player::toAttack() const {
  if (playerStrategy == nullptr)
    return new vector<Territory *>();

  return playerStrategy->toAttack();
}

/**
 * @brief Returns a list of territories that the player can defend.
 *        This method allows the player to select territories from their owned
 *        territories to defend.
 * @return A vector of Territory pointers that the player has chosen to defend.
 */
std::vector<Territory *> *Player::toDefend() const {
  if (playerStrategy == nullptr)
    return new vector<Territory *>();

  return playerStrategy->toDefend();
}

/**
 * @brief reading multiple choice from the user's input, each choice with
 * comma seperated.
 * @param minChoice minimum choice of the user eligible to choice.
 * @param maxChoice maximum choice of the user eligible to choice.
 * @param listSize
 * @param message a string message of the displaying as a header.
 */
std::vector<int> Player::readAllChoices(int minChoice, int maxChoice,
                                        int listSize, std::string message) {
  std::string input;
  std::vector<int> choices;
  while (true) {
    std::cout << message << endl;
    std::cin >> input; // Read input
    if (input == "q") {
      break; // Exit loop if 'q' is entered
    }
    try {
      int choice = std::stoi(input); // Convert input to int
      if (choice >= minChoice && choice <= maxChoice && choice < listSize)
        choices.push_back(choice);
      else
        std::cout << "Invalid choice. Please enter a number between "
                  << minChoice << " and " << maxChoice << ", or 'q' to quit.\n";
    } catch (const std::invalid_argument &e) {
      std::cout << "Invalid input. Please enter a number or 'q' to quit.\n";
    } catch (const std::out_of_range &e) {
      std::cout
          << "Number out of range. Please enter a number or 'q' to quit.\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  cout << "Selected choices are: ";
  for (int i : choices) {
    cout << to_string(i) + ", ";
  }
  cout << "\n" << endl;
  return choices;
}
