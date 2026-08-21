#ifndef PLAYER_H
#define PLAYER_H

#include "../Orders/Orders.h"
#include "../cards/Cards.h"
#include <iostream>
#include <string>
#include <vector>

class PlayerStrategy;

class Player {
private:
  /** Player's Name
   */
  std::string *playerName;
  /** The player's territories that owned
   */
  std::vector<Territory *> *territories;
  /** Player's attackTerritories list that
   */
  std::vector<Territory *> *allTerritories;
  /** Player's Card list
   */
  Hand *playerHand;
  /** Player's orders' list
   */
  OrdersList *orderslist;
  /** Number of army taht player able to used
   */
  int *armyPool;
  /** Players that this player has negotiated with this turn */
  std::vector<Player *> *negotiatedPlayers;
  /** Whether this player has conquered a territory this turn */
  bool *conqueredThisTurn;

  PlayerStrategy *playerStrategy;

public:
  enum class OrderType {
    EBomb,
    EDeploy,
    EBlockade,
    EAirlift,
    ENegotiate,
    EAdvance
  };
  void setPlayerStrategy(PlayerStrategy *strategy);

  PlayerStrategy *getPlayerStrategy() const;

  /**
   * @brief reading multiple choice from the user's input, each choice with
   * comma seperated.
   * @param minChoice minimum choice of the user eligible to choice.
   * @param maxChoice maximum choice of the user eligible to choice.
   * @param listSize
   * @param message a string message of the displaying as a header.
   */
  static std::vector<int> readAllChoices(int minChoice, int maxChoice,
                                         int listSize, std::string message);
  /**
   * @brief Default constructor
   * Player's name is created as 'Default Player'
   */
  Player();

  /// @brief constructor with strategy
  /// @param strategy for player
  Player(PlayerStrategy *strategy);

  /**
   * @brief parametirezed constructor
   * @param name the Player name.
   */
  Player(const std::string &name);

  /// @brief constructor with strategy and name
  /// @param name for player
  /// @param strategy for player
  Player(const string &name, PlayerStrategy *strategy);

  /**
   * @brief copy constructor
   * @param oldPlayer the Player's reference .
   */
  Player(const Player &oldPlayer);

  /**
   * Assignment constructor
   * @param oldPlayer copying previous Player object.
   */
  Player &operator=(const Player &oldPlayer);

  /**
   * deletion of the created pointers to prevent memory leaks.
   */
  ~Player();

  // Getter and setter for playerName
  std::string getPlayerName() const;
  void setPlayerName(const std::string &name);

  OrdersList *getOrderList() const;
  std::vector<Territory *> *getTerritories() const;

  // Getter and setter for attackTerritories
  void setAllTerritories(std::vector<Territory *> *allTerritories);
  std::vector<Territory *> *getAllTerritories() const;

  // Getter and setter for armyPool
  int *getArmyPool() const;
  void increaseArmyPool();
  void setArmyPool(int *armyPool);

  std::vector<Player *> *getNegotiatedPlayers() const;
  bool hasConcuredThisTurn() const;

  void addNegotiatedPlayer(Player *p);
  void clearNegotiations();
  void setConcuredThisTurn(bool val);

  bool isNegotiatedWith(Player *p) const;
  // Methods to manage territories
  /**
   * @brief Adding a new territory into the territories list
   * @param the pointer of the Territory object;
   */
  void addTerritory(Territory *territory);
  /**
   * @brief Removes an existing territory from the territories list
   * @param the pointer of the Territory object;
   */
  void removeTerritory(Territory *territory);
  /**
   * @brief Checks the provided territory is belongs to player or not
   * @param the pointer of the Territory object;
   */
  bool isTerritoryOwned(Territory *territory) const;

  // Getter for hand
  Hand *getHand() const;
  void setHand(Hand *hand);

  // Method to issue orders
  bool issueOrder(vector<Territory *> *defend, vector<Territory *> *attack,
                  int &committedArmies, Deck *deck, vector<Player *> *players);

  /**
   * @brief creates new Order based on the provided enum object.
   * @param OrderType enum class to identify what is the order's type.
   * @return Order object
   */
  Order *createOrder(OrderType) const;
  std::string enumToString(Player::OrderType type) const;

  /**
   * @brief Returns a list of territories that the player can attack.
   * @return A vector of Territory pointers that the player can attack.
   */
  std::vector<Territory *> *toAttack() const;
  /**
   * @brief Returns a list of territories that the player can defend.
   * @return A vector of Territory pointers that the player can defend.
   */
  std::vector<Territory *> *toDefend() const;
  friend std::ostream &operator<<(std::ostream &out, const Player &player);
};
#endif
