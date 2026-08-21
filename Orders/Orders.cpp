#include "Orders.h"
#include "../Player/Player.h"
#include "../map/map.h" ///Territory.h"
#include "iostream"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

/**
 * Default Base constructor
 */
Order::Order() {
  this->effect = new std::string("Default Order");
  this->orderType = new std::string("");
  this->executed = new bool(false);
  this->adjacencyList =
      new std::unordered_map<Territory *, std::vector<Territory *>>();
}

/**
 * @brief Parameterized constructor with default argument.
 * @param type The describe the type of order.
 */
Order::Order(const std::string &type) {
  this->orderType = new std::string(type);
  this->executed = new bool(false);
  this->effect = new std::string("");
  this->adjacencyList =
      new std::unordered_map<Territory *, std::vector<Territory *>>();
}

/**
 * @brief Copy Constructor.
 * @param type The reference of the previous base order.
 */
Order::Order(const Order &prevOrder) {
  this->orderType = new std::string(*prevOrder.orderType);
  this->effect = new std::string(*prevOrder.effect);
  this->executed = new bool(*prevOrder.executed);
  this->adjacencyList =
      new std::unordered_map<Territory *, std::vector<Territory *>>(
          *prevOrder.adjacencyList);
}

/**
 * @brief Assignment operator
 * @param type The reference of the previous base order.
 */
Order &Order::operator=(const Order &prevOrder) {
  if (this != &prevOrder) {
    delete orderType;
    delete effect;
    delete executed;
    delete adjacencyList; // Delete old adjacencyList

    this->orderType = new std::string(*prevOrder.orderType);
    this->effect = new std::string(*prevOrder.effect);
    this->executed = new bool(*prevOrder.executed);
    this->adjacencyList =
        new std::unordered_map<Territory *, std::vector<Territory *>>(
            *prevOrder.adjacencyList);
  }
  return *this;
}

// Deletion of the Order's pointers to prevent memory leaks.
Order::~Order() {
  delete orderType;
  delete effect;
  delete executed;
  delete adjacencyList;
}

// Getters
std::string *Order::getOrderType() const { return orderType; }
std::string *Order::getEffect() const { return effect; }
bool *Order::getExecuted() const { return executed; }
std::unordered_map<Territory *, std::vector<Territory *>> *
Order::getAdjacencyList() const {
  return adjacencyList;
}

// Setters
void Order::setExecuted(bool isExec) { *(this->executed) = isExec; }
void Order::setOrderType(const std::string &desc) { *(this->orderType) = desc; }
void Order::setEffect(const std::string &effect) { *(this->effect) = effect; }
void Order::setAdjacencyList(
    std::unordered_map<Territory *, std::vector<Territory *>> *adjacencyList) {
  this->adjacencyList = adjacencyList;
}

bool Order::isAdjacent(Territory *source, Territory *target) {
  if (adjacencyList == nullptr || source == nullptr || target == nullptr) {
    return false;
  }
  if (adjacencyList->count(source) > 0) {
    const std::vector<Territory *> &neighbors = adjacencyList->at(source);

    return std::find(neighbors.begin(), neighbors.end(), target) !=
           neighbors.end();
  }
  return false;
}
/**
 * @brief Stream operator to display attributes of the object.
 */
ostream &operator<<(ostream &out, const Order &order) {
  out << " Order Type: " << *(order.getOrderType());
  out << "\t\n\t| Effect: " << *(order.getEffect());
  out << "\n==============================================";
  return out;
}

/// @brief gives the order effect as a string
/// @return string of order effect
string Order::stringToLog() const { return "Order Executed: " + *effect; }

// ########## Deploying Order #############
// Default constructor
Deploy::Deploy() : Order("Deploy") {

  player = nullptr;
  target = nullptr;
  armiesNum = new int(1);
}

Deploy::~Deploy() { delete armiesNum; }
/**
 * @brief Parameterized constructor with default argument.
 * @param type The describe the type of order.
 */
Deploy::Deploy(Player *p, Territory *t, int *num)
    : Order("Deploy"), player(p), target(t), armiesNum(new int(*num)) {}

/**
 * @brief Copy Constructor.
 * @param other The reference of the previous base order.
 */
Deploy::Deploy(const Deploy &other)
    : Order(other), player(other.player), target(other.target),
      armiesNum(new int(*other.armiesNum)) {}

/**
 * @brief Assignment operator
 */
Deploy &Deploy::operator=(const Deploy &other) {
  if (this != &other) {
    Order::operator=(other);

    this->target = other.target;
    this->player = other.player;
    delete armiesNum;
    this->armiesNum = new int(*other.armiesNum);
  }
  return *this;
}

std::ostream &operator<<(std::ostream &out, const Deploy &deploy) {
  out << static_cast<const Order &>(deploy); // Call base class operator<<
  out << "\t  Player: " << deploy.player->getPlayerName()
      << "\t, Target Territory: " << deploy.target->getName()
      << "\t, Armies: " << *deploy.armiesNum;
  return out;
}

/**
 * @brief polymorphic clone the order object;
 */
Order *Deploy::clone() const { return new Deploy(*this); }

// Getters and Setters
Player *Deploy::getPlayer() const { return player; }
Territory *Deploy::getTerritory() const { return target; }
int *Deploy::getArmiesNum() const { return armiesNum; }

void Deploy::setPlayer(Player *player) { this->player = player; }
void Deploy::setTerritory(Territory *target) { this->target = target; }
void Deploy::setArmiesNum(int *arm) { this->armiesNum = arm; }

/**
 * @brief validates the current order eligible to execute.
 */
bool Deploy::validate() {
  if (!player || !target) {
    setEffect("You don't have player or target to deploy army.");
    return false;
  }
  // Player must own the target territory
  if (target->getOwner() != player) {
    setEffect("The target territory is not belong to you.");
    return false;
  }

  // WARN: Player has positive number of armies
  if (*armiesNum <= 0) {
    setEffect("You must deploy a positive number of armies.");
    return false;
  }

  // WARN: Player must have enough armies in their army pool
  if (*armiesNum > *player->getArmyPool()) {
    setEffect("You do not have enough armies in your pool to deploy " +
              to_string(*armiesNum) + " armies. However, you have " +
              to_string(*player->getArmyPool()) + " available.");
    return false;
  }
  return true;
}

/**
 * @brief executes the current order.
 */
void Deploy::execute() {
  if (!validate()) {
    Deploy::setEffect("Deploying Order Not Executed because of " +
                      *(Deploy::getEffect()) + ".");
    return;
  }

  int current = target->getArmyCount();
  target->setArmyCount(current + *armiesNum);
  *player->getArmyPool() = *player->getArmyPool() - *armiesNum;

  Deploy::setExecuted(true);
  Deploy::setEffect("Deployed " + to_string(*armiesNum) + " armies to " +
                    target->getName());

  Notify(this);
}
// ########## Advancing Order #############
// Default constructor
Advance::Advance() : Order("Advance") {
  player = nullptr;
  target = nullptr;
  source = nullptr;
  armiesNum = new int(1);
  deck = nullptr;
}

/**
 * @brief Parameterized constructor with default argument.
 * @param type The describe the type of order.
 */
Advance::Advance(Player *p, Territory *s, Territory *t, Deck *d, int *num)
    : Order("Advance"), player(p), source(s), target(t),
      armiesNum(new int(*num)), deck(d) {}

/**
 * @brief Copy Constructor.
 * @param other The reference of the previous base order.
 */
Advance::Advance(const Advance &other)
    : Order(other), player(other.player), source(other.source),
      target(other.target), armiesNum(new int(*other.armiesNum)),
      deck(other.deck) {}

/**
 * @brief Assignment operator
 */
Advance &Advance::operator=(const Advance &other) {
  if (this != &other) {
    Order::operator=(other);
    this->target = other.target;
    this->source = other.source;
    this->player = other.player;
    delete armiesNum;
    this->armiesNum = new int(*other.armiesNum);
    this->deck = other.deck;
  }
  return *this;
}

Advance::~Advance() { delete armiesNum; }

std::ostream &operator<<(std::ostream &out, const Advance &advance) {
  out << static_cast<const Order &>(advance); // Call base class operator<<
  out << "\t  Player: " << advance.getPlayer()->getPlayerName()
      << "\t, Target Territory: " << advance.getTargetTerritory()->getName()
      << "\t, Source Territory: " << advance.getSourceTerritory()->getName()
      << "\t, Armies: " << *advance.getArmiesNum();
  return out;
}
// Getters and Setters
Player *Advance::getPlayer() const { return player; }
Territory *Advance::getTargetTerritory() const { return target; }
Territory *Advance::getSourceTerritory() const { return source; }
int *Advance::getArmiesNum() const { return armiesNum; }

void Advance::setPlayer(Player *player) { this->player = player; }
void Advance::setTargetTerritory(Territory *target) { this->target = target; }
void Advance::setSourceTerritory(Territory *source) { this->source = source; }
void Advance::setArmiesNum(int *arm) { this->armiesNum = arm; }

/**
 * @brief polymorphic clone the order object;
 */
Order *Advance::clone() const { return new Advance(*this); }

bool Advance::validate() {
  // Player source or target pointes not be nullptr
  if (!player || !source || !target) {
    setEffect("You don't have player, source or target territory to advance an "
              "army.");
    return false;
  }

  // source's owner should be the provided player.
  if (source->getOwner() != player) {
    setEffect("The source territory is not belong to you.");
    return false;
  }

  if (getAdjacencyList()->empty()) {
    setEffect("You must set the adjacentTerritory list before applying Advance "
              "order.");
    return false;
  }

  if (!isAdjacent(source, target)) {
    setEffect("Selected Territory is not an adjacent of the source territory.");
    return false;
  }
  if (target->getOwner() != player &&
      player->isNegotiatedWith(target->getOwner())) {
    setEffect("Cannot attack " + target->getName() +
              ": peace treaty is active with " +
              target->getOwner()->getPlayerName());
    return false;
  }
  return true;
}

/**
 * @brief executes the current order.
 */
void Advance::execute() {
  if (!validate()) {
    Advance::setEffect(*(Advance::getEffect()) +
                       " Advancing Order Not Executed.");
    return;
  }

  if (player == target->getOwner()) {
    int targerArmy = target->getArmyCount();
    int sourceArm = source->getArmyCount();
    source->setArmyCount(source->getArmyCount() - *armiesNum);
    target->setArmyCount(target->getArmyCount() + *armiesNum);
    Advance::setExecuted(true);
    Advance::setEffect("Moved" + to_string(*armiesNum) + " armies from " +
                       source->getName() + " to " + target->getName());
    return;
  }

  // Attacking Simulation
  int attackerArmCount = *armiesNum;
  int defenderArmCount = target->getArmyCount();
  int attackLoss = 0;
  int defendLoss = 0;

  // Each attacker has 60% chance to kill one defender
  for (int i = 0; i < attackerArmCount; i++) {
    if (rand() % 100 < 60) {
      defendLoss++;
    }
  }
  // Each attacker has 70% chance to kill one defender
  for (int i = 0; i < defenderArmCount; i++) {
    if (rand() % 100 < 70) {
      attackLoss++;
    }
  }

  attackLoss = std::min(attackLoss, attackerArmCount);
  defendLoss = std::min(defendLoss, defenderArmCount);

  int remainingAttackers = attackerArmCount - attackLoss;
  int remainingDefenders = defenderArmCount - defendLoss;

  source->setArmyCount(source->getArmyCount() - *armiesNum);

  if (remainingDefenders <= 0) {
    Player *oldOwner = target->getOwner();
    if (oldOwner) {
      oldOwner->removeTerritory(target);
    }
    target->setOwner(player);
    target->setArmyCount(remainingAttackers);
    player->addTerritory(target);

    // Give player a card if this is their first conquest this turn
    if (deck != nullptr && !player->hasConcuredThisTurn()) {
      deck->draw(player->getHand());
      player->setConcuredThisTurn(true);
    }

    Advance::setExecuted(true);
    Advance::setEffect("Attack on " + to_string(*armiesNum) +
                       " armies success. " + to_string(remainingDefenders) +
                       " defenders remain. " + "Attacker lost " +
                       to_string(attackLoss) + " armies");

  } else {
    target->setArmyCount(max(0, remainingDefenders));
    Advance::setExecuted(true);
    Advance::setEffect("Attack on " + target->getName() + " failed");
  }
  Notify(this);
}

// #################### Bombing ####################
Bomb::Bomb() : Order("Bomb") {
  player = nullptr;
  target = nullptr;
}
/**
 * @brief Parameterized constructor with default argument.
 * @param type The describe the type of order.
 */
Bomb::Bomb(Player *p, Territory *t)
    : Order("Bomb"), player(p), target(t) {
} // Parameterized constructor initializes armiesNum

/**
 * @brief Copy Constructor.
 * @param other The reference of the previous base order.
 */
Bomb::Bomb(const Bomb &other)
    : Order(other), player(other.player), target(other.target) {}
/**
 * @brief Assignment operator
 */
Bomb &Bomb::operator=(const Bomb &other) {
  if (this != &other) {
    Order::operator=(other);

    this->target = other.target;
    this->player = other.player;
  }
  return *this;
}

std::ostream &operator<<(std::ostream &out, const Bomb &bomb) {
  out << static_cast<const Order &>(bomb); // Call base class operator<<
  out << "\t  Player: " << bomb.getPlayer()->getPlayerName()
      << "\t, Target Territory: " << bomb.getTargetTerritory()->getName();
  return out;
}

Bomb::~Bomb() {}
// Getters and Setters
Player *Bomb::getPlayer() const { return player; }
Territory *Bomb::getTargetTerritory() const { return target; }

void Bomb::setPlayer(Player *player) { this->player = player; }
void Bomb::setTargetTerritory(Territory *target) { this->target = target; }
/**
 * @brief polymorphic clone the order object;
 */
Order *Bomb::clone() const { return new Bomb(*this); }

/**
 * @brief validates the current order eligible to execute.
 */
bool Bomb::validate() {
  // Player and Target pointers must not be nullptr.
  if (!player || !target) {
    setEffect("You don't have player or target territory.");
    return false;
  }
  // Player cannot own the target territory to bomb it
  if (target->getOwner() == player) {
    setEffect("You cannot bomb your own territory.");
    return false;
  }
  bool adjacentToOwned = false;
  for (Territory *owned : *player->getTerritories()) {
    if (isAdjacent(owned, target)) {
      adjacentToOwned = true;
      break;
    }
  }
  if (!adjacentToOwned) {
    setEffect("Target territory is not adjacent to any of your territories.");
    return false;
  }
  return true;
}

/**
 * @brief executes the current order.
 */
void Bomb::execute() {
  if (!validate()) {
    Bomb::setEffect(*(Bomb::getEffect()) + " Bombing Order Not Executed.");
    return;
  }

  // Bomb order destroys a half of armies.
  int half = target->getArmyCount() / 2;

  // Destroying half of the armies
  target->setArmyCount(target->getArmyCount() - half);

  Bomb::setExecuted(true);
  Bomb::setEffect("Bomb destroyed " + to_string(half) + " armies on " +
                  target->getName() + " territory. Remaining armies: " +
                  to_string(target->getArmyCount()) + ".");
  Notify(this);
}

// ########## Blockading Order #############
Blockade::Blockade() : Order("Blockade") {
  neutralPlayer = nullptr;
  issuingPlayer = nullptr;
  target = nullptr;
}

/**
 * @brief Parameterized constructor with default argument.
 * @param type The describe the type of order.
 */
Blockade::Blockade(Player *p, Territory *t)
    : Order("Blockade"), issuingPlayer(p), neutralPlayer(nullptr), target(t) {}

/**
 * @brief Copy Constructor.
 * @param other The reference of the previous base order.
 */
Blockade::Blockade(const Blockade &other)
    : Order(other), issuingPlayer(other.issuingPlayer),
      neutralPlayer(other.neutralPlayer), target(other.target) {}
/**
 * @brief Assignment operator
 */
Blockade &Blockade::operator=(const Blockade &other) {
  if (this != &other) {
    Order::operator=(other);
    this->target = other.target;
    this->neutralPlayer = other.neutralPlayer;
    this->issuingPlayer = other.issuingPlayer;
  }
  return *this;
}

Blockade::~Blockade() {}

/**
 * @brief Stream operator to display attributes of the object.
 */
std::ostream &operator<<(std::ostream &out, const Blockade &blockade) {
  out << static_cast<const Order &>(blockade); // Call base class operator<<
  out << "\t  Player: " << blockade.getPlayer()->getPlayerName()
      << "\t, Target Territory: " << blockade.getTargetTerritory()->getName();
  return out;
}

/**
 * @brief polymorphic clone the order object;
 */
Order *Blockade::clone() const { return new Blockade(*this); }

// Getters and Setters
Player *Blockade::getPlayer() const { return issuingPlayer; }
Territory *Blockade::getTargetTerritory() const { return target; }

void Blockade::setPlayer(Player *player) { this->issuingPlayer = player; }
void Blockade::setTargetTerritory(Territory *target) { this->target = target; }

/**
 * @brief validates the current order eligible to execute.
 */
bool Blockade::validate() {
  // Player and Target pointers must not be nullptr.
  if (!issuingPlayer || !target) {
    Blockade::setEffect("The player or target are not provided.");
    return false;
  }

  // Player cannot own the target territory to blockade it
  if (target->getOwner() != issuingPlayer) {
    Blockade::setEffect(
        "The provided Player does not owner of the target territory.");
    return false;
  }

  return true;
}

/**
 * @brief executes the current order.
 */
void Blockade::execute() {
  if (!validate()) {
    Blockade::setEffect(*(Blockade::getEffect()) +
                        " Blockade Order Not Executed.");
    return;
  }

  // doubled armies
  target->setArmyCount(target->getArmyCount() * 2);

  issuingPlayer->removeTerritory(target);

  neutralPlayer = new Player("Neutral");

  // Transferred ownership
  target->setOwner(neutralPlayer);
  neutralPlayer->addTerritory(target);

  Blockade::setExecuted(true);
  Blockade::setEffect("Blockade " + to_string(target->getArmyCount()) +
                      " armies to " + target->getName());
  Notify(this);
}

// ########## Airlifting Order #############
Airlift::Airlift() : Order("Airlift") {
  player = nullptr;
  target = nullptr;
  source = nullptr;
  armiesNum = new int(1);
}
/**
 * @brief Parameterized constructor with default argument.
 * @param type The describe the type of order.
 */
Airlift::Airlift(Player *p, Territory *s, Territory *t, int *num)
    : Order("Airlift"), player(p), source(s), target(t),
      armiesNum(new int(*num)) {}

/**
 * @brief Copy Constructor.
 * @param other The reference of the previous base order.
 */
Airlift::Airlift(const Airlift &other)
    : Order(other), player(other.player), source(other.source),
      target(other.target), armiesNum(new int(*other.armiesNum)) {}

/**
 * @brief Assignment operator
 */
Airlift &Airlift::operator=(const Airlift &other) {
  if (this != &other) {
    Order::operator=(other);

    this->target = other.target;
    this->player = other.player;
    delete armiesNum;
    this->armiesNum = new int(*other.armiesNum);
    this->source = other.source;
  }
  return *this;
}

std::ostream &operator<<(std::ostream &out, const Airlift &airlift) {
  out << static_cast<const Order &>(airlift); // Call base class operator<<
  out << "\t  Player: " << airlift.getPlayer()->getPlayerName()
      << "\t, Target Territory: " << airlift.getTargetTerritory()->getName()
      << "\t, Source Territory: " << airlift.getSourceTerritory()->getName()
      << "\t, Armies: " << *airlift.getArmiesNum();
  return out;
}

/**
 * @brief polymorphic clone the order object;
 */
Order *Airlift::clone() const { return new Airlift(*this); }

Airlift::~Airlift() { delete armiesNum; }

// Getters and Setters
Player *Airlift::getPlayer() const { return player; }
Territory *Airlift::getTargetTerritory() const { return target; }
Territory *Airlift::getSourceTerritory() const { return source; }
int *Airlift::getArmiesNum() const { return armiesNum; }

void Airlift::setPlayer(Player *player) { this->player = player; }
void Airlift::setTargetTerritory(Territory *target) { this->target = target; }
void Airlift::setSourceTerritory(Territory *source) { this->source = source; }
void Airlift::setArmiesNum(int *arm) { this->armiesNum = arm; }

/**
 * @brief validates the current order eligible to execute.
 *  @note: validate function with simpliest way;
 */
bool Airlift::validate() {
  // Player, Source and Target musn't nullptr.
  if (!player || !source || !target) {
    Airlift::setEffect("The Player or source or target not provided.");
    return false;
  }
  // source and target territory should be the same player.
  bool isValid = target->getOwner() == player && source->getOwner() == player;
  if (!isValid)
    setEffect("Source or Target territory not belongs to provided player.");
  return isValid;
}

/**
 * @brief executes the current order.
 */
void Airlift::execute() {
  if (!validate()) {
    Airlift::setEffect(*(Airlift::getEffect()) +
                       " Airlift Order Not Executed.");
    return;
  }

  source->setArmyCount(source->getArmyCount() - *armiesNum);
  target->setArmyCount(target->getArmyCount() + *armiesNum);

  Airlift::setExecuted(true);
  // TODO: update the message based on the execution
  Airlift::setEffect("Airlift " + to_string(*armiesNum) + " armies to " +
                     target->getName());
  Notify(this);
}

// ########## Negotiating Order #############
Negotiate::Negotiate() : Order("Negotiate") {}
/**
 * @brief Parameterized constructor with default argument.
 * @param type The describe the type of order.
 */
Negotiate::Negotiate(Player *p, Player *t)
    : Order("Negotiate"), player(p), target(t) {}

/**
 * @brief Copy Constructor.
 * @param other The reference of the previous base order.
 */
Negotiate::Negotiate(const Negotiate &other)
    : Order(other), player(other.player), target(other.target) {}

/**
 * @brief Assignment operator
 */
Negotiate &Negotiate::operator=(const Negotiate &other) {
  if (this != &other) {
    Order::operator=(other);

    this->target = other.target;
    this->player = other.player;
  }
  return *this;
}

std::ostream &operator<<(std::ostream &out, const Negotiate &negotiate) {
  out << static_cast<const Order &>(negotiate); // Call base class operator<<
  out << "\t  Source Player: " << negotiate.getPlayer()->getPlayerName()
      << "\t, Target Player: " << negotiate.getTargetPlayer()->getPlayerName();
  return out;
}

/**
 * @brief polymorphic clone the order object;
 */
Order *Negotiate::clone() const { return new Negotiate(*this); }

Negotiate::~Negotiate() {}

// Getters and Setters
Player *Negotiate::getPlayer() const { return player; }
Player *Negotiate::getTargetPlayer() const { return target; }

void Negotiate::setPlayer(Player *player) { this->player = player; }
void Negotiate::setTargetPlayer(Player *target) { this->target = target; }
/**
 * @brief validates the current order eligible to execute.
 * @note: validate function with simpliest way;
 */
bool Negotiate::validate() {
  // Player and Target should not be nullptr.
  if (!player || !target)
    setEffect("Player or Target Player are not provided.");
  //
  // Player cannot negotiate with own
  bool isValid = player && target && player != target;
  if (!isValid)
    Negotiate::setEffect("Provided players are same person, so you cannot "
                         "negotiation with yourself.");
  return isValid;
}

/**
 * @brief executes the current order.
 */
void Negotiate::execute() {
  if (!validate()) {
    Negotiate::setEffect(*(Negotiate::getEffect()) +
                         " Negotiate Order Not Executed.");

    return;
  }

  player->addNegotiatedPlayer(target);
  target->addNegotiatedPlayer(player);
  Negotiate::setExecuted(true);
  Negotiate::setEffect("Negotiation success between " +
                       player->getPlayerName() + " and " +
                       target->getPlayerName() +
                       ". Attacks between them are blocked for this turn.");
  Notify(this);
}

// @@@@@@@@@@@@@@@@@@@@@@@ ORDERSLIST @@@@@@@@@@@@@@@@@@@@@@@@@@@@
/**
 * @brief Default constructor.
 */
OrdersList::OrdersList() {
  orders = new vector<Order *>();
  lastOrder = nullptr;
}

/**
 * @brief Copy Constructor.
 * @param other The reference of the previous base order.
 */
OrdersList::OrdersList(const OrdersList &other) {
  orders = new std::vector<Order *>();
  for (Order *ord : *other.orders) {
    orders->push_back(ord->clone()); // Use clone() for polymorphic copy
  }
}

/**
 * @brief operator Constructor and Desctructor of orders
 */
OrdersList &OrdersList::operator=(const OrdersList &other) {
  if (this != &other) {
    // Clean up existing orders
    for (Order *o : *orders) {
      delete o;
    }
    delete orders;

    // Deep copy new orders using clone()
    orders = new std::vector<Order *>();
    for (Order *o : *other.orders) {
      orders->push_back(o->clone());
    }
  }
  return *this;
}

// Getter of ordersList.
const std::vector<Order *> OrdersList::getOrders() const { return *orders; }

/**
 * @brief deletion of the ordersList pointers
 */
OrdersList::~OrdersList() {
  for (Order *order : *orders) {
    delete order; // Deallocate each Order object
  }
  delete orders; // Deallocate the vector itself
  lastOrder = nullptr;
}

/**
 * @brief Stream operator to display attributes of the object.
 */
ostream &operator<<(ostream &out, const OrdersList &ordersList) {
  out << "OrdersList contains " << ordersList.orders->size()
      << " orders:" << std::endl;
  for (Order *order : *ordersList.orders) {
    out << "\t- " << *order << std::endl;
  }
  return out;
}

/**
 * @brief Adding order into the Order list.
 * @param order pointer of the an order to the adding to the list
 */
void OrdersList::addOrder(Order *order) {
  orders->push_back(order);
  lastOrder = order;
  Notify(this);
}

/// @brief gives the last order type as a string
/// @return string of last order effect
string OrdersList::stringToLog() const {
  return "Order Issued: " + *lastOrder->getOrderType();
}

/**
 * @brief changes the order index into the ordersList.
 * @param order pointer of the an order to the removing to the list
 */
void OrdersList::move(int from, int to) {
  if (from < orders->size() && to >= 0 && to < orders->size()) {
    Order *tmp = orders->at(from);
    orders->erase(orders->begin() + from);
    orders->insert(orders->begin() + to, tmp);
  }
}

/**
 * @brief Removes order into the Order list.
 * @param order pointer of the an order to the removing to the list
 */
void OrdersList::removeOrder(int index) {
  if (index >= 0 && index < orders->size()) {
    delete orders->at(index);
    orders->erase(orders->begin() + index);
  }
}

// Getter
int OrdersList::getSize() const { return orders->size(); }
