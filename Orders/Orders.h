#ifndef ORDER_H
#define ORDER_H

#include "../LoggingObserver/LoggingObserver.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class Territory;
class Player;
class Deck;
class Order : public ILoggable, public Subject {

private:
  // They can be protected but for this level of assignment I applied private.
  // type of the order.
  std::string *orderType;
  // how effected the order after executed or validated.
  std::string *effect;
  // to be sure the order is executed or not if the user wants to see only
  // executed orders.
  bool *executed;
  std::unordered_map<Territory *, std::vector<Territory *>> *adjacencyList;

public:
  /**
   * Base default constructor
   */
  Order();
  /**
   * @brief base order creation with descsription of the order
   * @param desc Order's descsription how it will work
   */
  Order(const std::string &desc);
  /**
   * Copy constructor
   * @param other copying previous Orderr object.
   */
  Order(const Order &prevOrder);
  /**
   * Assignment constructor
   * @param other copying previous base Order object.
   */
  Order &operator=(const Order &prevOrder);

  /**
   * deletion of the created pointers to prevent memory leaks.
   */
  virtual ~Order();

  // Getters for private attributes
  std::string *getOrderType() const;
  std::string *getEffect() const;
  bool *getExecuted() const;
  std::unordered_map<Territory *, std::vector<Territory *>> *
  getAdjacencyList() const;

  // Setters for private attributes
  void setOrderType(const std::string &desc);
  void setEffect(const std::string &effect);
  void setExecuted(bool isExecuted);
  void setAdjacencyList(
      std::unordered_map<Territory *, std::vector<Territory *>> *adjacencyList);

  bool isAdjacent(Territory *source, Territory *target);
  // virtual methods for the validate in child class. Definitions are here but
  // implementation of this methods based on the order's functionality
  virtual bool validate() = 0;
  virtual void execute() = 0;

  // Polymorphic copying subclass of order
  virtual Order *clone() const = 0;

  friend std::ostream &operator<<(std::ostream &out, const Order &order);

  string stringToLog() const override;
};

// ############# DEPLOY ##################
class Deploy : public Order {
private:
  /**
   * @brief A pointer of the issuing the Deploy order
   */
  Player *player;
  /**
   * @brief A pointer of the targetting to Territory.
   */
  Territory *target;
  /**
   * @brief A int number of armies to join deploy
   */
  int *armiesNum;

public:
  /** @brief Default constructor
   */
  Deploy();
  /**
   * @param player the issuing player.
   * @param target the targetting Territory.
   * @param num number of armies
   */
  Deploy(Player *player, Territory *tar, int *num);
  /**
   * Assignment constructor
   * @param other copying previous Deploy object.
   */
  Deploy &operator=(const Deploy &other);
  /**
   * Copy constructor
   * @param other copying previous Deploy object.
   */
  Deploy(const Deploy &other);

  ~Deploy();

  // GEtters and Setters
  Player *getPlayer() const;
  void setPlayer(Player *player);
  Territory *getTerritory() const;
  void setTerritory(Territory *territory);
  int *getArmiesNum() const;
  void setArmiesNum(int *num);
  /**
   * Validates current Deploy order object.
   */
  bool validate() override;
  /**
   * @brief Executes the Deploy order object between two players.
   * @note The current implementation[ there is  no a specific requirement, so
   * the current state just implement based on my understanding and expectation
   * returns with a successfull message.
   */
  void execute() override;
  /**
   * Polymorphic copying order
   */
  Order *clone() const override;

  friend std::ostream &operator<<(std::ostream &out, const Deploy &deploy);
};

// ############# ADVANCE ##################
class Advance : public Order {
private:
  /**
   * @brief A pointer of the issuing player for the Advance order
   */
  Player *player;
  /**
   * @brief A pointer to the targetting for the Advance order.
   */
  Territory *target;
  /**
   * @brief A pointer to the sourcing of the Advance order.
   */
  Territory *source;
  /**
   * @brief A integer number of the moving armies from source to target
   * territory.
   */
  int *armiesNum;

  Deck *deck;

public:
  /** @brief Default constructor
   */
  Advance();
  /**
   * @param player the issuing player.
   * @param target the targetting territory.
   * @param source the sourcing territory.
   * @int num number of the armies
   */
  Advance(Player *player, Territory *source, Territory *tar, Deck *d, int *num);

  /**
   * Copy constructor
   * @param other copying previous Advance object.
   */
  Advance &operator=(const Advance &other);
  /**
   * Assignment constructor
   * @param other copying previous Advance object.
   */
  Advance(const Advance &other);

  ~Advance();

  // Getters and Setters
  Player *getPlayer() const;
  void setPlayer(Player *player);
  Territory *getTargetTerritory() const;
  void setTargetTerritory(Territory *territory);
  Territory *getSourceTerritory() const;
  void setSourceTerritory(Territory *territory);
  int *getArmiesNum() const;
  void setArmiesNum(int *num);

  /**
   * Validates current Advance order object.
   */
  bool validate() override;
  /**
   * @brief Executes the aiflifting order object between two territories.
   * @note The current implementation there is  no a specific requirement, so
   * the current state just arbitrary returns with a successfull negotiation
   * message.
   */
  void execute() override;

  /**
   * Polymorphic copying order.
   */
  Order *clone() const override; // Added clone method

  friend std::ostream &operator<<(std::ostream &out, const Advance &advance);
};

// ############# BOMB ##################
class Bomb : public Order {
private:
  /**
   * @brief A pointer of the issuing the Bomb order
   */
  Player *player;
  /**
   * @brief A pointer of the targetting to Territory.
   */
  Territory *target;

public:
  /** @brief Default constructor
   */
  Bomb();
  /**
   * @param player the issuing player.
   * @param target the targetting Territory.
   * @param num number of armies
   */
  Bomb(Player *player, Territory *ter);
  /**
   * Copy constructor
   * @param other copying previous Bomb object.
   */
  Bomb(const Bomb &other);

  /**
   * Assignment constructor
   * @param other copying previous Blockade object.
   */
  Bomb &operator=(const Bomb &other);

  ~Bomb();
  // Getters and Setters
  Player *getPlayer() const;
  void setPlayer(Player *player);
  Territory *getTargetTerritory() const;
  void setTargetTerritory(Territory *territory);

  /**
   * Validates current negotiation order object.
   */
  bool validate() override;

  /**
   * @brief Executes the negotiation order object between two players.
   * @note The current implementation[ there is  no a specific requirement, so
   * the current state just implement based on my understanding and expectation
   * returns with a successfull message.
   */
  void execute() override;

  /**
   * Polymorphic copying order
   */
  Order *clone() const override;

  friend std::ostream &operator<<(std::ostream &out, const Bomb &bomb);
};

// ############# BLOCKADE ##################
class Blockade : public Order {
private:
  /**
   * @brief A pointer of the issuing the Blockade order
   */
  Player *issuingPlayer;
  /**
   * @brief A pointer of the issuing the Blockade order
   */
  Player *neutralPlayer;
  /**
   * @brief A pointer of the targetting to Territory.
   */
  Territory *target;

public:
  /** @brief Default constructor
   */
  Blockade();

  /**
   * @param player the issuing player.
   * @param target the targetting Territory.
   */
  Blockade(Player *issuingPlayer, Territory *ter);

  /**
   * Copy constructor
   * @param other copying previous Blockade object.
   */
  Blockade(const Blockade &other);

  /**
   * Assignment constructor
   * @param other copying previous Blockade object.
   */
  Blockade &operator=(const Blockade &other);

  ~Blockade();
  // Getters and Setters
  Player *getPlayer() const;
  void setPlayer(Player *issuingPlayer);
  Territory *getTargetTerritory() const;
  void setTargetTerritory(Territory *territory);

  /**
   * Validates current negotiation order object.
   */
  bool validate() override;

  /**
   * @brief Executes the negotiation order object between two players.
   * @note The current implementation[ there is  no a specific requirement, so
   * the current state just implement based on my understanding and expectation
   * returns with a successfull message.
   */
  void execute() override;

  /**
   * Polymorphic copying order
   */
  Order *clone() const override;

  friend std::ostream &operator<<(std::ostream &out, const Blockade &blockade);
};

// ############# AIRLIFT##################
class Airlift : public Order {
private:
  /**
   * @brief A pointer of the issuing the airlifing order
   */
  Player *player;

  /**
   * @brief A pointer to the targetting for the airlifing order.
   */
  Territory *target;

  /**
   * @brief A pointer to the sourcing of the airlifting order.
   */
  Territory *source;

  /**
   * @brief A integer number of the moving armies from source to target
   * territory.
   */
  int *armiesNum;

public:
  /** @brief Default constructor
   */
  Airlift();
  /**
   * @param player the issuing player.
   * @param target the targetting territory.
   * @param source the sourcing territory.
   */
  Airlift(Player *player, Territory *source, Territory *target, int *num);

  /**
   * Copy constructor
   * @param other copying previous Airlift object.
   */
  Airlift(const Airlift &other);

  /**
   * Assignment constructor
   * @param other copying previous Airlift object.
   */
  Airlift &operator=(const Airlift &other);

  ~Airlift();
  // Getters and Setters
  Player *getPlayer() const;
  void setPlayer(Player *player);
  Territory *getTargetTerritory() const;
  void setTargetTerritory(Territory *territory);
  Territory *getSourceTerritory() const;
  void setSourceTerritory(Territory *territory);
  int *getArmiesNum() const;
  void setArmiesNum(int *num);

  /**
   * Validates current aiflift order object.
   */
  bool validate() override;

  /**
   * @brief Executes the aiflifting order object between two territories.
   * @note The current implementation there is  no a specific requirement, so
   * the current state just arbitrary returns with a successfull negotiation
   * message.
   */
  void execute() override;
  /**
   * Polymorphic copying order.
   */
  Order *clone() const override;

  friend std::ostream &operator<<(std::ostream &out, const Airlift &aiflift);
};

// ############# NEGOTIATE ##################
class Negotiate : public Order {
private:
  /**
   * @brief A pointer of the issuing the negotiation order
   */
  Player *player;

  /**
   * @brief A pointer of the targetting to negotiate with.
   */
  Player *target;

public:
  /** @brief Default constructor
   */
  Negotiate();

  /**
   * @param player the issuing player.
   * @param target the targetting player.
   */
  Negotiate(Player *player, Player *target);

  /**
   * Copy constructor
   * @param other copying previous Negotiate object.
   */
  Negotiate(const Negotiate &other);

  /**
   * Assignment constructor
   * @param other copying previous Negotiate object.
   */
  Negotiate &operator=(const Negotiate &other);

  ~Negotiate();
  // Getters and Setters
  Player *getPlayer() const;
  void setPlayer(Player *player);
  Player *getTargetPlayer() const;
  void setTargetPlayer(Player *target);

  /**
   * Validates current negotiation order object.
   */
  bool validate() override;

  /**
   * @brief Executes the negotiation order object between two players.
   * @note The current implementation[ there is  no a specific requirement and
   * condition how 2 player negotiate, so the current state just arbitrary
   * returns with a successfull negotiation message.
   */
  void execute() override;

  /**
   * Polymorphic copy object
   */
  Order *clone() const override;

  friend std::ostream &operator<<(std::ostream &out,
                                  const Negotiate &negotiate);
};

// ################ ORDERSLIST #########################
class OrdersList : public ILoggable, public Subject {
private:
  /**
   * @brief A pointer to a dynamically allocated vector containing Order
   * pointers.
   * * This structure requires manual allocation in the constructor (new)
   * and manual deallocation in the destructor (delete).
   */
  std::vector<Order *> *orders;

  /**
   * @brief A pointer to the most recently added Order.
   * * Used for quick access or tracking; does not own the memory
   * (the memory is managed by the orders vector).
   */
  Order *lastOrder;

public:
  /**
   * @brief Default constructor for OrdersList.
   * Initializes an empty vector of Order pointers.
   */
  OrdersList();
  /**
   * @brief Copy constructor for OrdersList.
   * Performs a deep copy of all Order pointers in the 'other' list.
   * @param other The OrdersList to copy from.
   */
  OrdersList(const OrdersList &other);
  /**
   * @brief Assignment operator for OrdersList.
   * Cleans up existing memory and performs a deep copy of the 'other' list.
   * @param other The OrdersList to assign from.
   * @return Reference to this OrdersList.
   */
  OrdersList &operator=(const OrdersList &other);
  /**
   * @brief Destructor for OrdersList.
   * Iterates through the vector and deletes all Order pointers to prevent
   * memory leaks.
   */
  ~OrdersList();

  /**
   * @brief Stream insertion operator for OrdersList.
   * Overloaded to display the contents of the orders list.
   * @param out The output stream.
   * @param ordersList The list to display.
   * @return Reference to the output stream.
   */
  friend std::ostream &operator<<(std::ostream &out,
                                  const OrdersList &ordersList);

  /**
   * @brief Adds a new order to the end of the player's order list.
   * * @param order A pointer to an Order object (or subclass like Deploy, Bomb,
   * etc.).
   * @note This method takes ownership of the pointer; the OrdersList will
   * handle deletion during destruction to prevent memory leaks.
   * @return void
   */
  void addOrder(Order *order);
  /**
   * @brief Removes order within orderslist based on the orders's index
   * * @param index The current 0-based position of the order.
   * @return void
   */
  void removeOrder(int index);
  /**
   * @brief Moves an order within the list to a new position.
   * * @param oldIndex The current 0-based position of the order.
   * @param newIndex The target 0-based position for the order.
   * @return void
   */
  void move(int from, int to);
  int getSize() const;

  /**
   * @brief Provides read-only access to the player's current ordersList.
   * * @return A constant vector of Order pointers.
   * @note The 'const' at the end of the signature ensures this method
   * does not modify the state of the OrdersList object.
   */
  const std::vector<Order *> getOrders() const;

  string stringToLog() const override;
};
#endif
