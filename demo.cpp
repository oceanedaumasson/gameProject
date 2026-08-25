/**
 * @file demo.cpp
 * @brief Showcase of the Warzone engine
 *
 * Each section below is a shortened,
 * non-interactive version of the corresponding driver so the whole thing
 * can run start to finish with zero input.
 *
 * Run from the build/ directory: ./demo
 */

#include "GameEngine/GameEngine.h"
#include "Player/Player.h"
#include "PlayerStrategy/PlayerStrategies.h"
#include "cards/Cards.h"
#include "map/map.h"
#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

using namespace std;

static const string MAPS_DIR = "../maps/";

void pause(const string &msg = "\nPress ENTER to continue...") {
  cout << msg;
  cin.clear();
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
  cin.get();
}

// ============================================================
// 1. MAP LOADING & VALIDATION
// ============================================================
void runMapDemo() {
  cout << "\n===== Map Loading & Validation =====\n" << endl;

  vector<string> files = {"canada.map", "europe.map", "germany.map",
                          "solar.map"};
  MapLoader loader;

  for (const string &f : files) {
    cout << "-- Loading " << f << " --" << endl;
    Map *map = loader.loadMap(MAPS_DIR + f);
    if (!map) {
      cout << "   Failed to load file." << endl;
      continue;
    }
    bool valid = map->validate();
    cout << "   Map name       : " << map->getMapName() << endl;
    cout << "   Territories    : " << map->getTerritories()->size() << endl;
    cout << "   Valid (connected graph, connected continents): "
        << (valid ? "YES" : "NO") << endl
        << endl;
    delete map;
  }
}

// ============================================================
// 2. CARDS
// ============================================================
void runCardsDemo() {
  cout << "\n===== Cards: Deck / Hand / Play =====\n" << endl;

  Deck *deck = new Deck();
  for (int i = 0; i < 3; i++) {
    deck->addCard(new Card(CardType::Bomb));
    deck->addCard(new Card(CardType::Reinforcement));
    deck->addCard(new Card(CardType::Blockade));
    deck->addCard(new Card(CardType::Airlift));
    deck->addCard(new Card(CardType::Diplomacy));
  }
  cout << "Deck created with " << deck->size() << " cards." << endl;

  Player *player = new Player("Jane Doe");
  Hand *hand = player->getHand();

  cout << "\nDrawing 5 cards into hand..." << endl;
  for (int i = 0; i < 5; i++) {
    Card *drawn = deck->draw(hand);
    cout << "  Drew " << *drawn << endl;
  }
  cout << "\nHand (" << hand->size() << " cards):\n" << *hand << endl;

  cout << "Playing every card in hand (each returns to the deck, "
          "creating an order where applicable)..."
      << endl;
  while (!hand->isEmpty()) {
    Card *cardToPlay = hand->getCardAt(0);
    cardToPlay->play(hand, deck, player);
  }

  cout << "\nPlayer's order list after playing hand:\n"
      << *player->getOrderList() << endl;
  cout << "Deck size after cards returned: " << deck->size() << endl;

  delete player;
  delete deck;
}

// ============================================================
// 3. ORDERS: all 6 types, validation, battle simulation
// ============================================================
void runOrdersDemo() {
  cout << "\n===== Orders: Types, Validation, Battle Simulation =====\n"
      << endl;

  Player *arwen = new Player("Arwen");
  Player *aragorn = new Player("Aragorn");

  Territory *quebec = new Territory(1, "Quebec", 1);
  Territory *ontario = new Territory(2, "Ontario", 1);
  Territory *alberta = new Territory(3, "Alberta", 1);
  Territory *manitoba = new Territory(4, "Manitoba", 1);

  quebec->setArmyCount(10);
  ontario->setArmyCount(7);
  alberta->setArmyCount(5);
  manitoba->setArmyCount(4);

  arwen->addTerritory(quebec);
  arwen->addTerritory(ontario);
  arwen->addTerritory(manitoba);
  quebec->setOwner(arwen);
  ontario->setOwner(arwen);
  manitoba->setOwner(arwen);

  aragorn->addTerritory(alberta);
  alberta->setOwner(aragorn);

  arwen->setArmyPool(new int(15));

  cout << "-- Creating and executing all 6 order types --\n" << endl;

  Order *deployOrder = new Deploy(arwen, quebec, new int(3));
  Order *advanceOrder =
      new Advance(arwen, quebec, ontario, nullptr, new int(2));
  Order *bombOrder = new Bomb(aragorn, quebec);
  Order *blockadeOrder = new Blockade(arwen, manitoba);
  Order *airliftOrder = new Airlift(arwen, ontario, quebec, new int(3));
  Order *negotiateOrder = new Negotiate(arwen, aragorn);

  auto *adjAdv = new unordered_map<Territory *, vector<Territory *>>();
  (*adjAdv)[quebec] = {ontario};
  advanceOrder->setAdjacencyList(adjAdv);

  auto *adjBomb = new unordered_map<Territory *, vector<Territory *>>();
  (*adjBomb)[alberta] = {quebec};
  bombOrder->setAdjacencyList(adjBomb);

  deployOrder->execute();
  cout << "Deploy    -> " << *deployOrder->getEffect() << endl;
  advanceOrder->execute();
  cout << "Advance   -> " << *advanceOrder->getEffect() << endl;
  bombOrder->execute();
  cout << "Bomb      -> " << *bombOrder->getEffect() << endl;
  blockadeOrder->execute();
  cout << "Blockade  -> " << *blockadeOrder->getEffect() << endl;
  airliftOrder->execute();
  cout << "Airlift   -> " << *airliftOrder->getEffect() << endl;
  negotiateOrder->execute();
  cout << "Negotiate -> " << *negotiateOrder->getEffect() << endl;

  cout << "\n-- Invalid order rejection --\n" << endl;

  Deploy *invDeploy = new Deploy(arwen, alberta, new int(3));
  cout << "Deploy to enemy territory: ";
  cout << (invDeploy->validate() ? "accepted (unexpected)"
                                 : "REJECTED - " + *invDeploy->getEffect())
      << endl;
  delete invDeploy;

  Bomb *invBomb = new Bomb(aragorn, alberta);
  cout << "Bomb own territory: ";
  cout << (invBomb->validate() ? "accepted (unexpected)"
                               : "REJECTED - " + *invBomb->getEffect())
      << endl;
  delete invBomb;

  cout << "\n-- Battle simulation: conquest awards a card --\n" << endl;

  srand(42);

  Player *legolas = new Player("Legolas");
  Player *sauron = new Player("Sauron");
  Territory *shire = new Territory(10, "Shire", 1);
  Territory *mordor = new Territory(11, "Mordor", 1);
  shire->setArmyCount(30);
  mordor->setArmyCount(1);
  shire->setOwner(legolas);
  mordor->setOwner(sauron);
  legolas->addTerritory(shire);
  sauron->addTerritory(mordor);
  legolas->setArmyPool(new int(50));

  Deck *deck = new Deck();
  deck->addCard(new Card(CardType::Bomb));

  auto *adjConquest = new unordered_map<Territory *, vector<Territory *>>();
  (*adjConquest)[shire] = {mordor};
  Advance *conquestOrder =
      new Advance(legolas, shire, mordor, deck, new int(25));
  conquestOrder->setAdjacencyList(adjConquest);

  cout << "Before: Mordor owned by " << mordor->getOwner()->getPlayerName()
      << " with " << mordor->getArmyCount() << " armies." << endl;
  conquestOrder->execute();
  cout << "Effect: " << *conquestOrder->getEffect() << endl;
  cout << "After:  Mordor owned by " << mordor->getOwner()->getPlayerName()
      << ". Legolas hand now has " << legolas->getHand()->size()
      << " card(s) (reward for conquering)." << endl;

  cout << "\n-- Negotiate blocks attacks for the rest of the turn --\n"
      << endl;

  Player *elvish = new Player("Elvish");
  Player *orcish = new Player("Orcish");
  Territory *rivendell = new Territory(20, "Rivendell", 2);
  Territory *isengard = new Territory(21, "Isengard", 2);
  rivendell->setArmyCount(10);
  isengard->setArmyCount(8);
  rivendell->setOwner(elvish);
  isengard->setOwner(orcish);
  elvish->addTerritory(rivendell);
  orcish->addTerritory(isengard);

  Negotiate *truce = new Negotiate(elvish, orcish);
  truce->execute();
  cout << "Negotiate -> " << *truce->getEffect() << endl;

  auto *adjElvish = new unordered_map<Territory *, vector<Territory *>>();
  (*adjElvish)[rivendell] = {isengard};
  Advance *blockedAttack =
      new Advance(elvish, rivendell, isengard, nullptr, new int(8));
  blockedAttack->setAdjacencyList(adjElvish);
  blockedAttack->execute();
  cout << "Elvish attempts to attack Isengard anyway -> "
      << *blockedAttack->getEffect() << endl;
  cout << "Isengard owner unchanged: "
      << isengard->getOwner()->getPlayerName() << endl;

  // Cleanup
  delete deployOrder;
  delete advanceOrder;
  delete bombOrder;
  delete blockadeOrder;
  delete airliftOrder;
  delete negotiateOrder;
  delete arwen;
  delete aragorn;
  delete quebec;
  delete ontario;
  delete alberta;
  delete manitoba;

  delete conquestOrder;
  delete deck;
  delete legolas;
  delete sauron;
  delete shire;
  delete mordor;

  delete truce;
  delete blockedAttack;
  delete elvish;
  delete orcish;
  delete rivendell;
  delete isengard;
}

// ============================================================
// 4. PLAYER STRATEGIES: Strategy pattern, runtime swap
// ============================================================
void runStrategyDemo() {
  cout << "\n===== Player Strategies (Strategy Pattern) =====\n" << endl;

  MapLoader loader;

  // --- Aggressive vs Aggressive ---
  {
    cout << "-- Aggressive strategy: always attacks its strongest holdings --"
        << endl;
    Map *map = loader.loadMap(MAPS_DIR + "solar.map");
    GameEngine engine;
    engine.setMap(map);

    Player *p1 = new Player("Player One");
    Player *p2 = new Player("Player Two");
    p1->setHand(new Hand());
    p2->setHand(new Hand());
    p1->setArmyPool(new int(10));
    p2->setArmyPool(new int(10));
    p1->setPlayerStrategy(new AggressivePlayerStrategy(p1, map));
    p2->setPlayerStrategy(new AggressivePlayerStrategy(p2, map));
    engine.setPlayers(new vector<Player *>{p1, p2});

    vector<Territory *> *all = map->getTerritories();
    int counts1[] = {3, 4, 10};
    int counts2[] = {10, 5, 4};
    for (int i = 0; i < 3; i++) {
      (*all)[i]->setOwner(p1);
      (*all)[i]->setArmyCount(counts1[i]);
      p1->addTerritory((*all)[i]);
      (*all)[i + 3]->setOwner(p2);
      (*all)[i + 3]->setArmyCount(counts2[i]);
      p2->addTerritory((*all)[i + 3]);
    }

    Deck *deck = new Deck();
    engine.reinforcementPhase();
    engine.issueOrdersPhase();
    engine.executeOrdersPhase();
    delete deck;
  }

  // --- Benevolent ---
  {
    cout << "\n-- Benevolent strategy: reinforces weakest holdings, never "
            "attacks --"
        << endl;
    Map *map = loader.loadMap(MAPS_DIR + "solar.map");
    GameEngine engine;
    engine.setMap(map);

    Player *p1 = new Player("Player One");
    Player *p2 = new Player("Player Two");
    p1->setHand(new Hand());
    p2->setHand(new Hand());
    p1->setArmyPool(new int(10));
    p2->setArmyPool(new int(10));
    p1->setPlayerStrategy(new BenevolentPlayerStrategy(p1, map));
    p2->setPlayerStrategy(new BenevolentPlayerStrategy(p2, map));
    engine.setPlayers(new vector<Player *>{p1, p2});

    vector<Territory *> *all = map->getTerritories();
    for (int i = 0; i < (int)all->size(); i++) {
      Player *owner = (i % 2 == 0) ? p1 : p2;
      (*all)[i]->setOwner(owner);
      (*all)[i]->setArmyCount(i + 1);
      owner->addTerritory((*all)[i]);
    }

    engine.reinforcementPhase();
    engine.issueOrdersPhase();
    engine.executeOrdersPhase();
  }

  // --- Cheater ---
  {
    cout << "\n-- Cheater strategy: auto-conquers adjacent enemy "
            "territories --"
        << endl;
    Map *map = loader.loadMap(MAPS_DIR + "solar.map");
    GameEngine engine;
    engine.setMap(map);

    Player *cheater = new Player("Cheater");
    Player *enemy = new Player("Enemy");
    cheater->setHand(new Hand());
    enemy->setHand(new Hand());
    cheater->setArmyPool(new int(5));
    enemy->setArmyPool(new int(5));
    cheater->setPlayerStrategy(new CheaterPlayerStrategy(cheater, map));

    vector<Territory *> *all = map->getTerritories();
    for (int i = 0; i < (int)all->size(); i++) {
      Player *owner = (i % 2 == 0) ? cheater : enemy;
      (*all)[i]->setOwner(owner);
      (*all)[i]->setArmyCount(3);
      owner->addTerritory((*all)[i]);
    }

    cout << "Before: Cheater owns " << cheater->getTerritories()->size()
        << " territories, Enemy owns " << enemy->getTerritories()->size()
        << "." << endl;

    Deck *deck = new Deck();
    vector<Player *> *players = new vector<Player *>{cheater, enemy};
    vector<Territory *> *defend = cheater->getPlayerStrategy()->toDefend();
    vector<Territory *> *attack = cheater->getPlayerStrategy()->toAttack();
    int committed = 0;
    cheater->issueOrder(defend, attack, committed, deck, players);

    cout << "After:  Cheater owns " << cheater->getTerritories()->size()
        << " territories, Enemy owns " << enemy->getTerritories()->size()
        << "." << endl;

    delete defend;
    delete attack;
    delete players;
    delete deck;
    delete cheater;
    delete enemy;
  }

  // --- Dynamic strategy swap on the same player, mid-game ---
  {
    cout << "\n-- Dynamic swap: same player, Benevolent -> Aggressive mid-game "
            "--"
        << endl;
    Map *map = loader.loadMap(MAPS_DIR + "solar.map");
    GameEngine engine;
    engine.setMap(map);

    Player *p1 = new Player("Player One");
    p1->setHand(new Hand());
    p1->setArmyPool(new int(10));
    p1->setPlayerStrategy(new BenevolentPlayerStrategy(p1, map));
    engine.setPlayers(new vector<Player *>{p1});

    vector<Territory *> *all = map->getTerritories();
    int counts[] = {3, 4, 10};
    for (int i = 0; i < 3; i++) {
      (*all)[i]->setOwner(p1);
      (*all)[i]->setArmyCount(counts[i]);
      p1->addTerritory((*all)[i]);
    }

    cout << "Strategy: " << p1->getPlayerStrategy() << " (Benevolent)"
        << endl;
    engine.issueOrdersPhase();
    engine.executeOrdersPhase();

    p1->setPlayerStrategy(new AggressivePlayerStrategy(p1, map));
    cout << "\nSwitched strategy at runtime -> now Aggressive" << endl;
    engine.issueOrdersPhase();
    engine.executeOrdersPhase();
  }
}

// ============================================================
// 5. FULL GAME: startup -> reinforcement -> orders -> execution
//    Runs a few rounds automatically (Aggressive vs Benevolent)
//    so the demo never waits for human input
// ============================================================
void runFullGameDemo() {
  cout << "\n===== Full Game Loop (bounded automatic run) =====\n" << endl;

  MapLoader loader;
  Map *map = loader.loadMap(MAPS_DIR + "germany.map");
  if (!map || !map->validate()) {
    cout << "Could not load/validate germany.map" << endl;
    return;
  }

  GameEngine engine;
  engine.setMap(map);

  Player *p1 = new Player("Aggressive Bot");
  Player *p2 = new Player("Benevolent Bot");
  p1->setHand(new Hand());
  p2->setHand(new Hand());
  p1->setPlayerStrategy(new AggressivePlayerStrategy(p1, map));
  p2->setPlayerStrategy(new BenevolentPlayerStrategy(p2, map));
  engine.setPlayers(new vector<Player *>{p1, p2});

  vector<Territory *> *territories = map->getTerritories();
  for (int i = 0; i < (int)territories->size(); i++) {
    territories->at(i)->setArmyCount(5);
    Player *owner = (i % 2 == 0) ? p1 : p2;
    territories->at(i)->setOwner(owner);
    owner->addTerritory(territories->at(i));
  }
  p1->setAllTerritories(new vector<Territory *>(*territories));
  p2->setAllTerritories(new vector<Territory *>(*territories));

  const int ROUNDS = 3;
  for (int round = 1; round <= ROUNDS; round++) {
    cout << "\n--- Round " << round << " ---" << endl;
    engine.reinforcementPhase();
    cout << p1->getPlayerName() << " pool: " << *p1->getArmyPool()
        << " | " << p2->getPlayerName() << " pool: " << *p2->getArmyPool()
        << endl;

    engine.issueOrdersPhase();
    engine.executeOrdersPhase();

    cout << p1->getPlayerName() << " owns "
        << p1->getTerritories()->size() << " territories | "
        << p2->getPlayerName() << " owns " << p2->getTerritories()->size()
        << " territories" << endl;
  }

  cout << "\n(Full mainGameLoop() runs the same phases until a single "
          "player owns every territory — bounded here to "
      << ROUNDS << " rounds for a quick demo.)" << endl;
}

// ============================================================
// MAIN MENU
// ============================================================
int readChoice(int minChoice, int maxChoice) {
  int choice;
  while (true) {
    cout << "Enter choice: ";
    if (cin >> choice && choice >= minChoice && choice <= maxChoice) {
      return choice;
    }
    cout << "Please enter a number between " << minChoice << " and "
        << maxChoice << ".\n";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
  }
}

int main() {
  int choice;
  do {
    cout << "\n============================================" << endl;
    cout << "           Warzone Engine — Demo           " << endl;
    cout << "============================================" << endl;
    cout << "1. Map loading & validation" << endl;
    cout << "2. Cards: deck, draw, play" << endl;
    cout << "3. Orders: all 6 types + battle simulation" << endl;
    cout << "4. Player strategies (Strategy pattern)" << endl;
    cout << "5. Full game loop (bounded automatic run)" << endl;
    cout << "0. Exit" << endl;

    choice = readChoice(0, 5);

    switch (choice) {
    case 1:
      runMapDemo();
      break;
    case 2:
      runCardsDemo();
      break;
    case 3:
      runOrdersDemo();
      break;
    case 4:
      runStrategyDemo();
      break;
    case 5:
      runFullGameDemo();
      break;
    case 0:
      cout << "\nGoodbye!" << endl;
      break;
    }
  } while (choice != 0);

  return 0;
}
