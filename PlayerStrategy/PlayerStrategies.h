#pragma once

#include "../Player/Player.h"
#include "../map/map.h"
#include <set>

/// @brief Abstract base class for player strategies (Strategy pattern).
/// Each concrete strategy implements different decision-making behaviour
/// for issueOrder(), toAttack(), and toDefend().
class PlayerStrategy {
protected:
  /// @brief Non-owning pointer to the player this strategy belongs to.
  Player *player;

public:
  /// @brief Default constructor. Sets player to nullptr.
  PlayerStrategy();

  /// @brief Constructor with player.
  /// @param player the player the base strategy acts for parametized player
  PlayerStrategy(Player *player);

  /// @brief Copy constructor. Deep copies the player pointer.
  /// @param other strategy to copy from
  PlayerStrategy(const PlayerStrategy &other);

  /// @brief Assignment operator. Deep copies the player pointer.
  /// @param other strategy to assign from
  /// @return reference to this strategy
  PlayerStrategy &operator=(const PlayerStrategy &other);

  /// @brief Stream insertion operator.
  friend ostream &operator<<(ostream &os, const PlayerStrategy &strategy);

  /// @brief Destructor.
  virtual ~PlayerStrategy();

  /// @brief Issue one order for this turn. Returns true if more orders should
  /// be issued, false when the player is done for this turn.
  virtual bool issueOrder(vector<Territory *> *defend,
                          vector<Territory *> *attack, int &committedArmies,
                          Deck *gameDeck, vector<Player *> *players) = 0;

  /// @brief Returns the list of enemy territories the player wants to attack.
  virtual vector<Territory *> *toAttack() = 0;

  /// @brief Returns the list of friendly territories the player wants to
  /// defend.
  virtual vector<Territory *> *toDefend() = 0;
};

// benevolent player strategy declaration
class BenevolentPlayerStrategy : public PlayerStrategy {
private:
  set<pair<Territory *, Territory *>>
      *reinforcedPairs; // to track pairs already reinforced this turn

protected:
  Map *map; // reference to the game map for adjacency checks
public:
  // constructors
  BenevolentPlayerStrategy();
  BenevolentPlayerStrategy(Player *player, Map *map);

  // copy constructor and assignment operator
  BenevolentPlayerStrategy(const BenevolentPlayerStrategy &other);
  BenevolentPlayerStrategy &operator=(const BenevolentPlayerStrategy &other);
  friend ostream &operator<<(ostream &os,
                             const BenevolentPlayerStrategy &strategy);

  // destructor
  ~BenevolentPlayerStrategy() override;

  // overridden methods
  bool issueOrder(vector<Territory *> *defend, vector<Territory *> *attack,
                  int &committedArmies, Deck *gameDeck,
                  vector<Player *> *players) override;
  vector<Territory *> *toAttack() override;
  vector<Territory *> *toDefend() override;

  void setMap(Map *map);
};

class AggressivePlayerStrategy : public PlayerStrategy {
protected:
  Map *map; // reference to the game map for adjacency checks
public:
  // Constructors
  AggressivePlayerStrategy();
  AggressivePlayerStrategy(Player *player, Map *map);
  AggressivePlayerStrategy(const AggressivePlayerStrategy &other);

  // Assignment operator
  AggressivePlayerStrategy &operator=(const AggressivePlayerStrategy &other);
  // Stream insertion operator
  friend ostream &operator<<(ostream &os,
                             const AggressivePlayerStrategy &strategy);

  // Destructor
  ~AggressivePlayerStrategy() override;

  // Overriden Methods
  bool issueOrder(vector<Territory *> *defend, vector<Territory *> *attack,
                  int &committedArmies, Deck *gameDeck,
                  vector<Player *> *players) override;
  vector<Territory *> *toAttack() override;
  vector<Territory *> *toDefend() override;
};

class NeutralPlayerStrategy : public PlayerStrategy {
protected:
  Map *map; // reference to the game map for adjacency checks

public:
  // Constructors
  NeutralPlayerStrategy();
  NeutralPlayerStrategy(Player *player, Map *map);
  NeutralPlayerStrategy(const NeutralPlayerStrategy &other);

  // Assignment operator
  NeutralPlayerStrategy &operator=(const NeutralPlayerStrategy &other);
  // Stream insertion operator
  friend ostream &operator<<(ostream &os,
                             const NeutralPlayerStrategy &strategy);

  // Destructor
  ~NeutralPlayerStrategy() override;

  // Overriden Methods
  bool issueOrder(vector<Territory *> *defend, vector<Territory *> *attack,
                  int &committedArmies, Deck *gameDeck,
                  vector<Player *> *players) override;
  vector<Territory *> *toAttack() override;
  vector<Territory *> *toDefend() override;
};

// ******************************************************************************************************************************
// CHEATER PLAYER
// ******************************************************************************************************************************

/// @brief Cheater strategy: automatically conquers all enemy territories
/// adjacent to the player's own territories once per turn, without issuing
/// any real orders.
class CheaterPlayerStrategy : public PlayerStrategy {
protected:
  Map *map;

public:
  CheaterPlayerStrategy();
  CheaterPlayerStrategy(Player *player, Map *map);
  CheaterPlayerStrategy(const CheaterPlayerStrategy &other);
  CheaterPlayerStrategy &operator=(const CheaterPlayerStrategy &other);
  ~CheaterPlayerStrategy() override;
  friend ostream &operator<<(ostream &os, const CheaterPlayerStrategy &cheater);

  /// @brief Directly transfers ownership of all territories in the attack
  /// list to this player. Returns false (done in a single call).
  bool issueOrder(vector<Territory *> *defend, vector<Territory *> *attack,
                  int &committedArmies, Deck *gameDeck,
                  vector<Player *> *players) override;

  /// @brief Returns all enemy territories adjacent to any territory owned by
  /// this player.
  vector<Territory *> *toAttack() override;

  /// @brief Returns all territories currently owned by this player.
  vector<Territory *> *toDefend() override;
};

// ******************************************************************************************************************************
// HUMAN PLAYER STRATEGY
// ******************************************************************************************************************************

/// @brief Human strategy: prompts the player interactively each turn to
/// select territories to defend/attack and which orders to issue.
class HumanPlayerStrategy : public PlayerStrategy {

protected:
  Map *map;

public:
  HumanPlayerStrategy();
  HumanPlayerStrategy(Player *player, Map *map);
  HumanPlayerStrategy(const HumanPlayerStrategy &other);
  HumanPlayerStrategy &operator=(const HumanPlayerStrategy &other);
  ~HumanPlayerStrategy() override;
  friend ostream &operator<<(ostream &os, const HumanPlayerStrategy &strategy);

  /// @brief Interactively prompts the human player to deploy, advance, or play
  /// a card. Returns true while the player has more orders to issue.
  bool issueOrder(vector<Territory *> *defend, vector<Territory *> *attack,
                  int &committedArmies, Deck *gameDeck,
                  vector<Player *> *players) override;

  /// @brief Prompts the human player to select which enemy territories to
  /// attack this turn.
  vector<Territory *> *toAttack() override;

  /// @brief Prompts the human player to select which of their territories to
  /// defend this turn.
  vector<Territory *> *toDefend() override;
};
