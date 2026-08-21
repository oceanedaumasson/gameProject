#include "./PlayerStrategies.h"
#include "../Orders/Orders.h"
#include <ostream>

using namespace std;

// ******************************************************************************************************************************
// PLAYER STRATEGY (abstract base)
// ******************************************************************************************************************************

/// @brief Default constructor. Sets player to nullptr.
PlayerStrategy::PlayerStrategy() : player(nullptr) {}

/// @brief Constructor with player.
/// @param player the player the base strategy acts for parametized player
PlayerStrategy::PlayerStrategy(Player *player) : player(player) {}

/// @brief Destructor. Nullifies the non-owning player pointer.
PlayerStrategy::~PlayerStrategy() { player = nullptr; }

/// @brief Copy constructor. Deep copies the player pointer.
/// @param other strategy to copy from
PlayerStrategy::PlayerStrategy(const PlayerStrategy &other)
    : player(new Player(*other.player)) {}

/// @brief Assignment operator. Deep copies the player pointer.
/// @param other strategy to assign from
/// @return reference to this strategy
PlayerStrategy &PlayerStrategy::operator=(const PlayerStrategy &other) {
  if (this != &other) {
    player = new Player(*other.player);
  }
  return *this;
}

/// @brief Stream insertion operator
ostream &operator<<(ostream &os, const PlayerStrategy &strategy) {
  os << "PlayerStrategy for player: "
     << (strategy.player ? strategy.player->getPlayerName() : "none");
  return os;
}

// ******************************************************************************************************************************
// Benevolent Player STRATEGY (derived from base)
// ******************************************************************************************************************************

BenevolentPlayerStrategy::BenevolentPlayerStrategy() : PlayerStrategy() {
  reinforcedPairs = new set<pair<Territory *, Territory *>>();
  map = nullptr;
}

BenevolentPlayerStrategy::BenevolentPlayerStrategy(Player *player, Map *map)
    : PlayerStrategy(player) {
  reinforcedPairs = new set<pair<Territory *, Territory *>>();
  this->map = map;
}

BenevolentPlayerStrategy::~BenevolentPlayerStrategy() {
  delete reinforcedPairs;
  reinforcedPairs = nullptr;
  map = nullptr;
}

BenevolentPlayerStrategy::BenevolentPlayerStrategy(
    const BenevolentPlayerStrategy &other)
    : PlayerStrategy(other) {
  reinforcedPairs =
      new set<pair<Territory *, Territory *>>(*other.reinforcedPairs);
  map = other.map;
}

BenevolentPlayerStrategy &
BenevolentPlayerStrategy::operator=(const BenevolentPlayerStrategy &other) {

  if (this != &other) {
    PlayerStrategy::operator=(other);
    delete reinforcedPairs;
    reinforcedPairs =
        new set<pair<Territory *, Territory *>>(*other.reinforcedPairs);
    map = other.map;
  }

  return *this;
}

ostream &operator<<(ostream &os, const BenevolentPlayerStrategy &strategy) {
  os << "Benevolent Player Strategy with player: " << strategy.player << endl;
  return os;
}

/// @brief returns list of territories to attack. Benevolent player never
/// attacks so it returns an empty list
/// @return empty list of territories to attack
vector<Territory *> *BenevolentPlayerStrategy::toAttack() {
  // benevolent player does not attack, so return an empty list
  return new vector<Territory *>();
}

/// @brief returns list of territories to defend. Benevolent player defends
/// weakest territories
/// @return list of territories to defend sorted from weakest to strongest
vector<Territory *> *BenevolentPlayerStrategy::toDefend() {
  // benevolent player defends its territories based on weakest to strongest
  // return a sorted list of territories based on army count (weakest to
  // strongest)

  vector<Territory *> *defendList =
      new vector<Territory *>(*player->getTerritories());

  // sort the list based on army count (weakest to strongest)
  sort(defendList->begin(), defendList->end(), [](Territory *a, Territory *b) {
    return a->getArmyCount() < b->getArmyCount();
  });

  return defendList;
}

bool BenevolentPlayerStrategy::issueOrder(vector<Territory *> *defend,
                                          vector<Territory *> *attack,
                                          int &committedArmies, Deck *gameDeck,
                                          vector<Player *> *players) {
  // benevolent player does not issue orders to attack, so it only issues deploy
  // orders to defend its weakest territories deploy all available armies to the
  // weakest territory

  bool issuedOrder = false;

  // deploy all available armies to the weakest territory
  if (*player->getArmyPool() > 0) {
    reinforcedPairs->clear(); // clear reinforced pairs at the start of the turn

    if (defend->empty()) {
      cout << "No defend territories selected" << endl;
      return false;
    }
    Territory *weakestTerritory =
        defend->front(); // get the weakest territory (first in the sorted list)
    Order *deployOrder =
        new Deploy(player, weakestTerritory, new int(*player->getArmyPool()));
    player->getOrderList()->addOrder(deployOrder);

    *player->getArmyPool() = 0; // reduce army pool by deployed armies

    cout << "Benevolent player deploys all army to defend weakest territory: "
         << defend->front()->getName() << endl;

    return true;
  }

  // if no armies to deploy, try to reinforce weaker territories by advancing
  // from stronger territories iterate through the defend list from strongest to
  // weakest and try to advance armies from stronger territories
  for (int i = defend->size() - 1; i >= 1; i--) {
    Territory *stronger = defend->at(i);

    // not enough army on stronger territory to advance
    if (stronger->getArmyCount() <= 1) {
      continue;
    }

    // find weaker adjacent territory to reinforce
    for (int j = 0; j < i; j++) {
      Territory *weaker = defend->at(j);

      if (reinforcedPairs->count({stronger, weaker})) {
        continue; // skip if this pair has already been reinforced this turn
      }

      if (map->checkAdjacent(stronger, weaker)) {
        int armiesToMove = stronger->getArmyCount() / 2;
        player->getOrderList()->addOrder(new Advance(
            player, stronger, weaker, gameDeck, new int(armiesToMove)));
        cout << "Benevolent player advances " << armiesToMove << " armies from "
             << stronger->getName()
             << " to reinforce weaker territory: " << weaker->getName() << endl;

        reinforcedPairs->insert(
            {stronger, weaker}); // mark this pair as reinforced
        return true; // move to next stronger territory after reinforcing one
                     // weaker territory
      }
    }
  }

  // airlift not adjacent territories if player has airlift card
  Hand *hand = player->getHand();

  // go through deck to find airlift cards
  for (int i = 0; i < hand->size(); i++) {
    Card *card = hand->getCardAt(i);
    if (card->getType() == CardType::Airlift) {

      // find the strongest territory to airlift from
      Territory *strongest = defend->back();

      // find the weakest territory to airlift to
      Territory *weakest = defend->front();

      // check if strongest territory has enough armies to airlift
      if (strongest->getArmyCount() <= 1) {
        break;
      }

      int armiesToMove = strongest->getArmyCount() / 2;
      player->getOrderList()->addOrder(
          new Airlift(player, strongest, weakest, new int(armiesToMove)));

      hand->removeCard(card);
      gameDeck->addCard(card);

      cout << "Benevolent player airlifts " << armiesToMove << " armies from "
           << strongest->getName()
           << " to reinforce weakest territory: " << weakest->getName() << endl;
      return true;
    }
  }
  return false;
}

/// @brief update the map variable for the strategy, used for adjacency checks
/// in toAttack and toDefend
/// @param map to set for the strategy
void BenevolentPlayerStrategy::setMap(Map *map) { this->map = map; }

// ******************************************************************************************************************************
// Aggressive Player STRATEGY (derived from base)
// ******************************************************************************************************************************

/// @brief default constructor
AggressivePlayerStrategy::AggressivePlayerStrategy() : PlayerStrategy() {
  map = nullptr;
}

/// @brief Parametized constructor
/// @param player to set
AggressivePlayerStrategy::AggressivePlayerStrategy(Player *player, Map *map)
    : PlayerStrategy(player) {
  this->map = map;
}

/// @brief aggressive copy constructor
/// @param other strategy to copy from
AggressivePlayerStrategy::AggressivePlayerStrategy(
    const AggressivePlayerStrategy &other)
    : PlayerStrategy(other) {
  map = other.map;
}

/// @brief aggressive assignment operator
/// @param other strategy to assign from
/// @return assigned strategy
AggressivePlayerStrategy &
AggressivePlayerStrategy::operator=(const AggressivePlayerStrategy &other) {
  if (this != &other) {
    PlayerStrategy::operator=(other);
    map = other.map;
  }

  return *this;
}

/// @brief aggressive stream insertion operator
/// @param os stream
/// @param strategy to print
/// @return stream
ostream &operator<<(ostream &os, const AggressivePlayerStrategy &strategy) {
  os << "Aggressive Player Strategy with player: "
     << strategy.player->getPlayerName() << endl;
  return os;
}

/// @brief aggressive destructor
AggressivePlayerStrategy::~AggressivePlayerStrategy() { map = nullptr; }

/// @brief Issue orders for aggressive player strategy
/// @param defend list of territories to defend
/// @param attack list of territories to attack
/// @param committedArmies number of armies already commited
/// @param gameDeck a pointer to the deck of cards in game
/// @param players player list of all players in game
/// @return true when order is issued, false to stop issuing
bool AggressivePlayerStrategy::issueOrder(vector<Territory *> *defend,
                                          vector<Territory *> *attack,
                                          int &committedArmies, Deck *gameDeck,
                                          vector<Player *> *players) {
  // Check if player still has territories
  if (defend->empty())
    return false;

  // DEPLOY PHASE: Deploy all armies to strongest territory
  Territory *strongest = defend->front();
  if (*player->getArmyPool() > 0) {
    int armies = *player->getArmyPool();

    player->getOrderList()->addOrder(
        new Deploy(player, strongest, new int(armies)));

    // Set pool to 0 to avoid infinite deploy
    *player->getArmyPool() = 0;
    return true;
  }

  // ADVANCE PHASE:
  // Move armies to strongest territory
  for (int i = 1; i < (int)defend->size(); i++) {
    Territory *t = defend->at(i);
    if (t->getArmyCount() > 0) {
      int armiesToMove = t->getArmyCount();
      player->getOrderList()->addOrder(
          new Advance(player, t, strongest, gameDeck, new int(armiesToMove)));
      defend->erase(defend->begin() + i);
      return true;
    }
  }

  // Attack from strongest territory
  if (!attack->empty() && strongest->getArmyCount() > 1) {
    Territory *target = attack->front();

    // Send all but one of strongest territory's army to attack
    int armiesToAdvance = strongest->getArmyCount() - 1;

    player->getOrderList()->addOrder(new Advance(
        player, strongest, target, gameDeck, new int(armiesToAdvance)));

    attack->erase(attack->begin());
    return true;
  }
  cout << endl;
  return false;
}

/// @brief Returns list of territories to attack
/// @return Pointer to a vector of territories to attack
vector<Territory *> *AggressivePlayerStrategy::toAttack() {
  vector<Territory *> *attackPriorityList = new vector<Territory *>();

  Territory *strongest = nullptr;
  // Loop to find strongest territory
  for (Territory *t : *player->getTerritories()) {
    if (strongest == nullptr || t->getArmyCount() > strongest->getArmyCount()) {
      strongest = t;
    }
  }

  if (strongest == nullptr)
    return attackPriorityList;

  // Get only the neighbors of strongest that are enemy territories
  const auto &adjList = *map->getAdjacencyList();
  auto it = adjList.find(strongest);
  if (it == adjList.end())
    return attackPriorityList;

  for (Territory *neighbor : *(it->second)) {
    if (!player->isTerritoryOwned(neighbor))
      attackPriorityList->push_back(neighbor);
  }

  return attackPriorityList;
}

/// @brief Returns list of territories to defend with the strongest territory
/// first
/// @return Pointer to a vector of the player's defend territories
vector<Territory *> *AggressivePlayerStrategy::toDefend() {
  vector<Territory *> *defendPriorityList = new vector<Territory *>();

  Territory *strongest = nullptr;
  // Loop to find strongest territory
  for (Territory *t : *player->getTerritories()) {
    if (strongest == nullptr || t->getArmyCount() > strongest->getArmyCount()) {
      strongest = t;
    }
  }

  // Put strongest territory at top of priority list
  if (strongest != nullptr) {
    defendPriorityList->push_back(strongest);
  }

  // Put remaining territories in list, unsorted
  for (Territory *t : *player->getTerritories()) {
    if (t != strongest) {
      defendPriorityList->push_back(t);
    }
  }
  return defendPriorityList;
}

// ******************************************************************************************************************************
// Neutral Player STRATEGY (derived from base)
// ******************************************************************************************************************************

/// @brief default constructor
NeutralPlayerStrategy::NeutralPlayerStrategy() : PlayerStrategy() {
  this->map = nullptr;
}

/// @brief Parametized constructor
/// @param player to set
NeutralPlayerStrategy::NeutralPlayerStrategy(Player *player, Map *map)
    : PlayerStrategy(player) {
  this->map = map;
}

/// @brief neutral copy constructor
/// @param other strategy to copy from
NeutralPlayerStrategy::NeutralPlayerStrategy(const NeutralPlayerStrategy &other)
    : PlayerStrategy(other) {
  map = other.map;
}

/// @brief neutral assignment operator
/// @param other strategy to assign from
/// @return assigned strategy
NeutralPlayerStrategy &
NeutralPlayerStrategy::operator=(const NeutralPlayerStrategy &other) {
  if (this != &other) {
    PlayerStrategy::operator=(other);
    map = other.map;
  }

  return *this;
}

/// @brief neutral stream insertion operator
/// @param os stream
/// @param strategy to print
/// @return stream
ostream &operator<<(ostream &os, const NeutralPlayerStrategy &strategy) {
  os << "Neutral Player Strategy with player: "
     << strategy.player->getPlayerName() << endl;
  return os;
}

/// @brief neutral destructor
NeutralPlayerStrategy::~NeutralPlayerStrategy() { map = nullptr; }

/// @brief Neutral player never issues an order
/// @param defend list of territories to defend
/// @param attack list of territories to attack
/// @param committedArmies number of armies already commited
/// @param gameDeck a pointer to the deck of cards in game
/// @param players player list of all players in game
/// @return Always return false since no orders are issued
bool NeutralPlayerStrategy::issueOrder(vector<Territory *> *defend,
                                       vector<Territory *> *attack,
                                       int &committedArmies, Deck *gameDeck,
                                       vector<Player *> *players) {
  return false;
}

/// @brief Returns an empty list, since neutral player never attacks
/// @return Pointer to an empty vector of territories
vector<Territory *> *NeutralPlayerStrategy::toAttack() {
  return new vector<Territory *>(); // returns empty list
}

/// @brief Returns all territories owned by the neutral player, not prioritized
/// @return List of the player's territories
vector<Territory *> *NeutralPlayerStrategy::toDefend() {
  // Create new list of player's territories
  vector<Territory *> *list = new vector<Territory *>();

  for (Territory *t : *player->getTerritories()) {
    list->push_back(t);
  }

  return list;
}

// ******************************************************************************************************************************
// Cheater Player STRATEGY (derived from base)
// ******************************************************************************************************************************

/// @brief Default constructor
CheaterPlayerStrategy::CheaterPlayerStrategy() : PlayerStrategy() {
  map = nullptr;
}

/// @brief Constructor with player and map.
/// @param player the player this strategy acts for (non-owning)
/// @param map the game map used for adjacency look-ups (non-owning)
CheaterPlayerStrategy::CheaterPlayerStrategy(Player *player, Map *map)
    : PlayerStrategy(player) {
  this->map = map;
}

/// @brief Copy constructor
CheaterPlayerStrategy::CheaterPlayerStrategy(const CheaterPlayerStrategy &other)
    : PlayerStrategy(other) {
  map = other.map;
}

/// @brief Assignment operator
CheaterPlayerStrategy &
CheaterPlayerStrategy::operator=(const CheaterPlayerStrategy &other) {
  if (this != &other) {
    PlayerStrategy::operator=(other);
    map = other.map;
  }
  return *this;
}

/// @brief Destructor
CheaterPlayerStrategy::~CheaterPlayerStrategy() { map = nullptr; }

/// @brief Stream insertion operator
ostream &operator<<(ostream &os, const CheaterPlayerStrategy &cheater) {
  os << "CheaterPlayerStrategy for player: "
     << (cheater.player ? cheater.player->getPlayerName() : "none");
  return os;
}

/// @brief Returns all territories currently owned by the player.
vector<Territory *> *CheaterPlayerStrategy::toDefend() {
  return new vector<Territory *>(*player->getTerritories());
}

/// @brief Returns all enemy territories adjacent to any territory owned by
/// this player, using the game map's adjacency list.
vector<Territory *> *CheaterPlayerStrategy::toAttack() {
  vector<Territory *> *attackList = new vector<Territory *>();
  const auto &adjList = *map->getAdjacencyList();

  for (Territory *owned : *player->getTerritories()) {
    auto it = adjList.find(owned);
    if (it == adjList.end())
      continue;
    for (Territory *neighbor : *(it->second)) {
      if (neighbor->getOwner() != player)
        attackList->push_back(neighbor);
    }
  }

  return attackList;
}

/// @brief Directly transfers ownership of every territory in the attack list
/// to this player. Removes each territory from its previous owner.
/// Returns false because the cheater is done after a single call.
bool CheaterPlayerStrategy::issueOrder(vector<Territory *> *defend,
                                       vector<Territory *> *attack,
                                       int &committedArmies, Deck *gameDeck,
                                       vector<Player *> *players) {
  for (Territory *t : *attack) {
    Player *previousOwner = t->getOwner();
    if (previousOwner != nullptr) {
      previousOwner->removeTerritory(t);
    }
    t->setOwner(player);
    player->addTerritory(t);
    cout << player->getPlayerName() << " (Cheater) conquers " << t->getName()
         << endl;
  }
  return false; // Done in one call — no further orders needed
}

// ******************************************************************************************************************************
// HUMAN PLAYER STRATEGY
// ******************************************************************************************************************************

HumanPlayerStrategy::HumanPlayerStrategy() : PlayerStrategy() { map = nullptr; }

HumanPlayerStrategy::HumanPlayerStrategy(Player *player, Map *map)
    : PlayerStrategy(player) {
  this->map = map;
}

HumanPlayerStrategy::HumanPlayerStrategy(const HumanPlayerStrategy &other)
    : PlayerStrategy(other) {
  map = other.map;
}

HumanPlayerStrategy &
HumanPlayerStrategy::operator=(const HumanPlayerStrategy &other) {
  if (this != &other) {
    PlayerStrategy::operator=(other);
    map = other.map;
  }
  return *this;
}

HumanPlayerStrategy::~HumanPlayerStrategy() {}

ostream &operator<<(ostream &os, const HumanPlayerStrategy &strategy) {
  os << "HumanPlayerStrategy for player: "
     << (strategy.player ? strategy.player->getPlayerName() : "none");
  return os;
}

/// @brief Lets the human pick which of their territories to defend.
vector<Territory *> *HumanPlayerStrategy::toDefend() {
  vector<Territory *> *owned = player->getTerritories();
  vector<Territory *> *defendList = new vector<Territory *>();

  if (owned->empty()) {
    cout << "No territories to defend." << endl;
    return defendList;
  }

  cout << "\n"
       << player->getPlayerName() << " — select territories to defend:" << endl;
  for (int i = 0; i < (int)owned->size(); i++)
    cout << "  " << i << ": " << owned->at(i)->getName() << " ("
         << owned->at(i)->getArmyCount() << " armies)" << endl;

  vector<int> choices = Player::readAllChoices(
      0, (int)owned->size() - 1, (int)owned->size(),
      "Enter index (repeat to add more, 'q' to finish):");

  for (int idx : choices)
    defendList->push_back(owned->at(idx));

  return defendList;
}

/// @brief Lets the human pick which enemy territories to attack.
vector<Territory *> *HumanPlayerStrategy::toAttack() {
  vector<Territory *> *all = player->getAllTerritories();
  vector<Territory *> *enemyList = new vector<Territory *>();
  vector<Territory *> *attackList = new vector<Territory *>();

  for (Territory *t : *all)
    if (!player->isTerritoryOwned(t))
      enemyList->push_back(t);

  if (enemyList->empty()) {
    cout << "No enemy territories available to attack." << endl;
    delete enemyList;
    return attackList;
  }

  cout << "\n"
       << player->getPlayerName() << " — select territories to attack:" << endl;
  for (int i = 0; i < (int)enemyList->size(); i++)
    cout << "  " << i << ": " << enemyList->at(i)->getName() << " (owner: "
         << (enemyList->at(i)->getOwner()
                 ? enemyList->at(i)->getOwner()->getPlayerName()
                 : "none")
         << ")" << endl;

  vector<int> choices = Player::readAllChoices(
      0, (int)enemyList->size() - 1, (int)enemyList->size(),
      "Enter index (repeat to add more, 'q' to finish):");

  for (int idx : choices)
    attackList->push_back(enemyList->at(idx));

  delete enemyList;
  return attackList;
}

/// @brief Interactive order menu: deploy → advance to defend/attack → play card
/// → done.
bool HumanPlayerStrategy::issueOrder(vector<Territory *> *defend,
                                     vector<Territory *> *attack,
                                     int &committedArmies, Deck *gameDeck,
                                     vector<Player *> *players) {
  // Deploy phase: must spend all armies before anything else
  if (*player->getArmyPool() > 0) {
    if (defend->empty()) {
      cout << "No defend territories — skipping deploy." << endl;
      *player->getArmyPool() = 0;
      return false;
    }
    int armies = *player->getArmyPool();
    Territory *target = defend->front();
    player->getOrderList()->addOrder(
        new Deploy(player, target, new int(armies)));
    *player->getArmyPool() = 0;
    cout << player->getPlayerName() << " deploys " << armies << " armies to "
         << target->getName() << endl;
    return true;
  }

  // Order selection menu
  cout << "\n" << player->getPlayerName() << " — choose an order:" << endl;
  cout << "  1. Advance to defend" << endl;
  cout << "  2. Advance to attack" << endl;
  cout << "  3. Play a card" << endl;
  cout << "  4. Done issuing orders" << endl;
  cout << "Enter choice: ";

  int choice;
  cin >> choice;

  switch (choice) {
  case 1: {
    // Advance to defend
    vector<Territory *> *owned = player->getTerritories();
    if (owned->empty() || defend->empty()) {
      cout << "Nothing to advance." << endl;
      return true;
    }

    cout << "Source territory:" << endl;
    for (int i = 0; i < (int)owned->size(); i++)
      cout << "  " << i << ": " << owned->at(i)->getName() << " ("
           << owned->at(i)->getArmyCount() << " armies)" << endl;
    int src;
    do {
      cout << "Enter source: ";
      cin >> src;
    } while (src < 0 || src >= (int)owned->size());

    cout << "Target (defend) territory:" << endl;
    for (int i = 0; i < (int)defend->size(); i++)
      cout << "  " << i << ": " << defend->at(i)->getName() << endl;
    int tgt;
    do {
      cout << "Enter target: ";
      cin >> tgt;
    } while (tgt < 0 || tgt >= (int)defend->size());

    int maxArmies = owned->at(src)->getArmyCount();
    int num;
    do {
      cout << "Armies to move (1-" << maxArmies << "): ";
      cin >> num;
    } while (num < 1 || num > maxArmies);

    player->getOrderList()->addOrder(new Advance(
        player, owned->at(src), defend->at(tgt), gameDeck, new int(num)));
    return true;
  }
  case 2: {
    // Advance to attack
    vector<Territory *> *owned = player->getTerritories();
    if (owned->empty() || attack->empty()) {
      cout << "Nothing to attack." << endl;
      return true;
    }

    cout << "Source territory:" << endl;
    for (int i = 0; i < (int)owned->size(); i++)
      cout << "  " << i << ": " << owned->at(i)->getName() << " ("
           << owned->at(i)->getArmyCount() << " armies)" << endl;
    int src;
    do {
      cout << "Enter source: ";
      cin >> src;
    } while (src < 0 || src >= (int)owned->size());

    cout << "Target (attack) territory:" << endl;
    for (int i = 0; i < (int)attack->size(); i++)
      cout << "  " << i << ": " << attack->at(i)->getName() << endl;
    int tgt;
    do {
      cout << "Enter target: ";
      cin >> tgt;
    } while (tgt < 0 || tgt >= (int)attack->size());

    int maxArmies = owned->at(src)->getArmyCount();
    int num;
    do {
      cout << "Armies to move (1-" << maxArmies << "): ";
      cin >> num;
    } while (num < 1 || num > maxArmies);

    player->getOrderList()->addOrder(new Advance(
        player, owned->at(src), attack->at(tgt), gameDeck, new int(num)));
    return true;
  }
  case 3: {
    // Play a card
    Hand *hand = player->getHand();
    if (hand == nullptr || hand->size() == 0) {
      cout << "No cards in hand." << endl;
      return true;
    }

    cout << "Your hand:" << endl;
    for (int i = 0; i < hand->size(); i++)
      cout << "  " << i << ": " << *hand->getCardAt(i) << endl;

    int cardIdx;
    do {
      cout << "Choose card: ";
      cin >> cardIdx;
    } while (cardIdx < 0 || cardIdx >= hand->size());

    Card *card = hand->getCardAt(cardIdx);
    CardType type = card->getType();
    vector<Territory *> *owned = player->getTerritories();

    if (type == CardType::Airlift) {
      int src, tgt, num;
      cout << "Source:" << endl;
      for (int i = 0; i < (int)owned->size(); i++)
        cout << "  " << i << ": " << owned->at(i)->getName() << endl;
      do {
        cout << "Enter source: ";
        cin >> src;
      } while (src < 0 || src >= (int)owned->size());
      cout << "Target:" << endl;
      for (int i = 0; i < (int)owned->size(); i++)
        cout << "  " << i << ": " << owned->at(i)->getName() << endl;
      do {
        cout << "Enter target: ";
        cin >> tgt;
      } while (tgt < 0 || tgt >= (int)owned->size());
      do {
        cout << "Armies: ";
        cin >> num;
      } while (num < 1);
      player->getOrderList()->addOrder(
          new Airlift(player, owned->at(src), owned->at(tgt), new int(num)));
      hand->removeCard(card);
      gameDeck->addCard(card);

    } else if (type == CardType::Bomb) {
      if (attack->empty()) {
        cout << "No attack targets for bomb." << endl;
        return true;
      }
      cout << "Bomb target:" << endl;
      for (int i = 0; i < (int)attack->size(); i++)
        cout << "  " << i << ": " << attack->at(i)->getName() << endl;
      int tgt;
      do {
        cout << "Enter target: ";
        cin >> tgt;
      } while (tgt < 0 || tgt >= (int)attack->size());
      player->getOrderList()->addOrder(new Bomb(player, attack->at(tgt)));
      hand->removeCard(card);
      gameDeck->addCard(card);

    } else if (type == CardType::Blockade) {
      cout << "Blockade territory:" << endl;
      for (int i = 0; i < (int)owned->size(); i++)
        cout << "  " << i << ": " << owned->at(i)->getName() << endl;
      int tgt;
      do {
        cout << "Enter target: ";
        cin >> tgt;
      } while (tgt < 0 || tgt >= (int)owned->size());
      player->getOrderList()->addOrder(new Blockade(player, owned->at(tgt)));
      hand->removeCard(card);
      gameDeck->addCard(card);

    } else if (type == CardType::Diplomacy) {
      vector<Player *> others;
      cout << "Negotiate with:" << endl;
      for (Player *p : *players)
        if (p != player) {
          cout << "  " << others.size() << ": " << p->getPlayerName() << endl;
          others.push_back(p);
        }
      if (others.empty()) {
        cout << "No other players." << endl;
        return true;
      }
      int tgt;
      do {
        cout << "Enter choice: ";
        cin >> tgt;
      } while (tgt < 0 || tgt >= (int)others.size());
      player->getOrderList()->addOrder(new Negotiate(player, others.at(tgt)));
      hand->removeCard(card);
      gameDeck->addCard(card);

    } else {
      card->play(hand, gameDeck, player);
    }
    return true;
  }
  case 4:
    return false; // done issuing orders
  default:
    cout << "Invalid choice." << endl;
    return true;
  }
}
