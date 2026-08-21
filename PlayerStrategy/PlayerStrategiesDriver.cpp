/**
 * @file PlayerStrategiesDriver.cpp
 * @brief Tests BenevolentPlayerStrategy and CheaterPlayerStrategy:
 *
 *  Benevolent:
 *    - toAttack() always returns empty
 *    - toDefend() returns territories sorted weakest to strongest
 *    - issueOrder() deploys to weakest, then advances from strongest to weakest
 *
 *  Cheater:
 *    - toDefend() returns all territories owned by the cheater
 *    - toAttack() returns all enemy territories adjacent to the cheater's
 *    - issueOrder() directly transfers ownership of every attack territory
 */

#include "../GameEngine/GameEngine.h"
#include "../Player/Player.h"
#include "../cards/Cards.h"
#include "../map/map.h"
#include "./PlayerStrategies.h"
#include <iostream>

using namespace std;

void demoBenevolent() {
  cout << "----- Benevolent Player Strategy Demo -----" << endl;

  // Load and validate the map
  MapLoader *loader = new MapLoader();
  Map *map = loader->loadMap("../map/maps/solar.map");
  if (!map || !map->validate()) {
    cerr << "Failed to load solar.map" << endl;
    delete loader;
    delete map;
    return;
  }

  // Set up the game engine with the map
  GameEngine *engine = new GameEngine();
  engine->setMap(map);

  Player *p1 = new Player("Player 1");
  Player *p2 = new Player("Player 2");

  p1->setHand(new Hand());
  p2->setHand(new Hand());
  p1->getHand()->addCard(new Card(CardType::Airlift));
  p2->getHand()->addCard(new Card(CardType::Airlift));
  p1->setArmyPool(new int(0));
  p2->setArmyPool(new int(0));

  BenevolentPlayerStrategy *b1 = new BenevolentPlayerStrategy(p1, map);
  BenevolentPlayerStrategy *b2 = new BenevolentPlayerStrategy(p2, map);

  // Assign benevolent strategy to both
  p1->setPlayerStrategy(b1);
  p2->setPlayerStrategy(b2);

  // Register players with the engine
  engine->setPlayers(new vector<Player *>{p1, p2});

  // Distribute territories and seed armies so toDefend() sorting is meaningful
  vector<Territory *> *all = map->getTerritories();
  for (int i = 0; i < (int)all->size(); i++) {
    Territory *t = (*all)[i];
    Player *owner = (i % 2 == 0) ? p1 : p2;
    t->setOwner(owner);
    t->setArmyCount(i + 1); // unique army counts so sort order is obvious
    owner->addTerritory(t);
  }

  // Give players armies and draw cards
  Deck *deck = new Deck();
  *p1->getArmyPool() = 10;
  *p2->getArmyPool() = 10;
  deck->draw(p1->getHand());
  deck->draw(p2->getHand());

  // Run one full turn through the game engine phases
  engine->reinforcementPhase();
  engine->issueOrdersPhase();
  engine->executeOrdersPhase();

  delete deck;
  delete b1;
  delete b2;
  delete engine;
  delete loader;
}

void demoAggressive() {
  cout << "----- Aggressive Player Strategy Demo -----" << endl;

  // Load and validate map
  MapLoader loader;
  Map *map = loader.loadMap("../map/maps/solar.map");
  if (!map || !map->validate()) {
    cerr << "Failed to load solar.map" << endl;
    return;
  }

  // Set up the game engine
  GameEngine engine;
  engine.setMap(map);

  // Create aggressive player, plus neutral to test on
  Player *p1 = new Player("Player One");
  Player *p2 = new Player("Player Two");

  p1->setHand(new Hand());
  p2->setHand(new Hand());
  p1->setArmyPool(new int(10));
  p2->setArmyPool(new int(10));

  // Assign strategies to both
  p1->setPlayerStrategy(new AggressivePlayerStrategy(p1, map));
  p2->setPlayerStrategy(new AggressivePlayerStrategy(p2, map));

  // Register players
  engine.setPlayers(new vector<Player *>{p1, p2});

  // Give 3 territories to each player
  vector<Territory *> *all = map->getTerritories();
  Territory *t1 = (*all)[0];
  Territory *t2 = (*all)[1];
  Territory *t3 = (*all)[2];

  Territory *t4 = (*all)[3];
  Territory *t5 = (*all)[4];
  Territory *t6 = (*all)[5];

  t1->setOwner(p1);
  t1->setArmyCount(3);
  p1->addTerritory(t1);
  t2->setOwner(p1);
  t2->setArmyCount(4);
  p1->addTerritory(t2);
  t3->setOwner(p1);
  t3->setArmyCount(10);
  p1->addTerritory(t3);

  t4->setOwner(p2);
  t4->setArmyCount(10);
  p2->addTerritory(t4);
  t5->setOwner(p2);
  t5->setArmyCount(5);
  p2->addTerritory(t5);
  t6->setOwner(p2);
  t6->setArmyCount(4);
  p2->addTerritory(t6);

  // Give players armies and draw cards
  Deck *deck = new Deck();
  *p1->getArmyPool() = 10;
  *p2->getArmyPool() = 10;
  deck->draw(p1->getHand());
  deck->draw(p2->getHand());

  // Simulate one round
  engine.reinforcementPhase();
  engine.issueOrdersPhase();
  engine.executeOrdersPhase();

  cout << "\n=== Aggressive driver complete ===" << endl;

  delete deck;
}

void demoNeutral() {
  cout << "----- Neutral Player Strategy Demo -----" << endl;

  // Load and validate map
  MapLoader loader;
  Map *map = loader.loadMap("../map/maps/solar.map");
  if (!map || !map->validate()) {
    cerr << "Failed to load solar.map" << endl;
    return;
  }

  // Set up the game engine
  GameEngine engine;
  engine.setMap(map);

  // Create neutral and aggressive players
  Player *p1 = new Player("NeutralPlayer");
  Player *p2 = new Player("AggressivePlayer");

  p1->setHand(new Hand());
  p2->setHand(new Hand());

  p1->setArmyPool(new int(10));
  p2->setArmyPool(new int(10));

  // Assign strategies to both
  p1->setPlayerStrategy(new NeutralPlayerStrategy(p1, map));
  p2->setPlayerStrategy(new AggressivePlayerStrategy(p2, map));

  // Register players
  engine.setPlayers(new vector<Player *>{p1, p2});

  // Give territories to players
  vector<Territory *> *all = map->getTerritories();
  Territory *t1 = (*all)[0];
  Territory *t2 = (*all)[2];
  Territory *t3 = (*all)[6];
  Territory *t4 = (*all)[7];
  Territory *t5 = (*all)[1];
  Territory *t6 = (*all)[3];

  t1->setOwner(p1);
  t1->setArmyCount(5);
  p1->addTerritory(t1);
  t2->setOwner(p1);
  t2->setArmyCount(5);
  p1->addTerritory(t2);
  t3->setOwner(p1);
  t3->setArmyCount(10);
  p1->addTerritory(t3);
  t4->setOwner(p1);
  t4->setArmyCount(8);
  p1->addTerritory(t4);
  t5->setOwner(p2);
  t5->setArmyCount(10);
  p2->addTerritory(t5);
  t6->setOwner(p2);
  t6->setArmyCount(8);
  p2->addTerritory(t6);

  Deck *deck = new Deck();

  // Make priority lists
  vector<Territory *> *defend = p1->toDefend();
  vector<Territory *> *attack = p1->toAttack();

  // Show neutral player issues no orders
  cout << "\n--- First, test issueOrder directly on neutral player (should "
          "return no) ---"
       << endl;
  int committed = 0;
  cout << "Calling issueOrder for neutral player..." << endl;
  bool issued = p1->getPlayerStrategy()->issueOrder(defend, attack, committed,
                                                    deck, engine.getPlayers());
  cout << "Did neutral issue order? " << (issued ? "YES" : "NO") << endl;
  delete defend;
  delete attack;

  // Call main game loop, will play one round
  cout << "\nNow call main game loop..." << endl;
  engine.mainGameLoop();

  cout << "--- Verifying Neutral became Aggressive ---" << endl;
  vector<Territory *> *newAttack = p1->toAttack();
  vector<Territory *> *newDefend = p1->toDefend();
  committed = 0;

  // Try issuing an order now
  cout << "Calling issueOrder for neutral player (now aggressive)..." << endl;
  issued = p1->issueOrder(newDefend, newAttack, committed, deck,
                          engine.getPlayers());
  cout << "Does neutral player now issue orders? " << (issued ? "YES" : "NO")
       << endl
       << endl;

  cout << "\n=== Neutral driver complete ===" << endl;

  delete newDefend;
  delete newAttack;
  delete deck;
}

void switchingDemo() {
  cout << "----- Dynamic Strategy Switching Demo -----" << endl;

  // Load and validate map
  MapLoader *loader = new MapLoader();
  Map *map = loader->loadMap("../map/maps/solar.map");
  if (!map || !map->validate()) {
    cerr << "Failed to load solar.map" << endl;
    return;
  }

  // Set up the game engine
  GameEngine *engine = new GameEngine();
  engine->setMap(map);

  // Create a player with an initial strategy
  Player *p1 = new Player("Player One");
  p1->setHand(new Hand());
  p1->setArmyPool(new int(10));

  BenevolentPlayerStrategy *b1 = new BenevolentPlayerStrategy(p1, map);
  p1->setPlayerStrategy(b1);

  // Register player
  engine->setPlayers(new vector<Player *>{p1});

  vector<Territory *> *all = map->getTerritories();
  Territory *t1 = (*all)[0];
  Territory *t2 = (*all)[1];
  Territory *t3 = (*all)[2];

  t1->setOwner(p1);
  t1->setArmyCount(3);
  p1->addTerritory(t1);
  t2->setOwner(p1);
  t2->setArmyCount(4);
  p1->addTerritory(t2);
  t3->setOwner(p1);
  t3->setArmyCount(10);
  p1->addTerritory(t3);

  Deck *deck = new Deck();
  deck->draw(p1->getHand());

  cout << "\n--- Player starts with Benevolent strategy ---" << endl;
  engine->issueOrdersPhase();
  engine->executeOrdersPhase();

  cout << "\n--- Now switching to Aggressive strategy ---" << endl;

  AggressivePlayerStrategy *a1 = new AggressivePlayerStrategy(p1, map);
  p1->setPlayerStrategy(a1);

  engine->issueOrdersPhase();
  engine->executeOrdersPhase();

  delete deck;
  delete engine;
  delete loader;
  delete b1;
  delete a1;
}

// ******************************************************************************************************************************
// HUMAN PLAYER STRATEGY
// ******************************************************************************************************************************

void demoHuman() {
  cout << "----- Human Player Strategy Demo -----" << endl;

  MapLoader loader;
  Map *map = loader.loadMap("../map/maps/solar.map");
  if (!map || !map->validate()) {
    cerr << "Failed to load solar.map" << endl;
    return;
  }

  GameEngine engine;
  engine.setMap(map);

  // Human player vs a neutral opponent
  Player *human = new Player("Human");
  Player *neutral = new Player("Neutral");

  human->setHand(new Hand());
  neutral->setHand(new Hand());
  human->setArmyPool(new int(10));
  neutral->setArmyPool(new int(5));

  // Give human an Airlift card to demonstrate card play in the menu
  human->getHand()->addCard(new Card(CardType::Airlift));

  human->setPlayerStrategy(new HumanPlayerStrategy(human, map));
  neutral->setPlayerStrategy(new NeutralPlayerStrategy(neutral, map));

  engine.setPlayers(new vector<Player *>{human, neutral});

  // Distribute territories — human gets first half, neutral gets second half
  vector<Territory *> *all = map->getTerritories();
  int half = (int)all->size() / 2;
  for (int i = 0; i < (int)all->size(); i++) {
    Territory *t = (*all)[i];
    Player *owner = (i < half) ? human : neutral;
    t->setOwner(owner);
    t->setArmyCount(5);
    owner->addTerritory(t);
  }

  // Provide the full territory list so toAttack() can show enemy territories
  human->setAllTerritories(all);
  neutral->setAllTerritories(all);

  cout << "\nHuman player owns:" << endl;
  for (Territory *t : *human->getTerritories())
    cout << "  " << t->getName() << " (" << t->getArmyCount() << " armies)"
         << endl;

  cout << "\nNeutral player owns:" << endl;
  for (Territory *t : *neutral->getTerritories())
    cout << "  " << t->getName() << " (" << t->getArmyCount() << " armies)"
         << endl;

  cout << "\n=== Reinforcement Phase ===" << endl;
  engine.reinforcementPhase();

  cout << "\n=== Issue Orders Phase ===" << endl;
  cout << "(You will be prompted to select territories and issue orders)"
       << endl;
  engine.issueOrdersPhase();

  cout << "\n=== Execute Orders Phase ===" << endl;
  engine.executeOrdersPhase();

  cout << "\n=== Human driver complete ===" << endl;
}

// ******************************************************************************************************************************
// CHEATER PLAYER STRATEGY
// ******************************************************************************************************************************

void demoCheater() {
  // Load and validate the map
  MapLoader loader;
  Map *map = loader.loadMap("../map/maps/solar.map");
  if (!map || !map->validate()) {
    cerr << "Failed to load solar.map" << endl;
    return;
  }

  // Set up the game engine with the map
  GameEngine *engine = new GameEngine();
  engine->setMap(map);

  cout << "\n\n========================================" << endl;
  cout << "    CHEATER PLAYER STRATEGY TEST" << endl;
  cout << "========================================" << endl;

  // Create cheater and enemy players
  Player *cheater = new Player("Cheater");
  Player *enemy = new Player("Enemy");
  cheater->setHand(new Hand());
  enemy->setHand(new Hand());
  cheater->setArmyPool(new int(5));
  enemy->setArmyPool(new int(5));

  // Assign cheater strategy
  CheaterPlayerStrategy *cheaterStrategy =
      new CheaterPlayerStrategy(cheater, map);
  cheater->setPlayerStrategy(cheaterStrategy);

  // Distribute territories: even indices to cheater, odd to enemy
  vector<Territory *> *all2 = map->getTerritories();
  for (int i = 0; i < (int)all2->size(); i++) {
    Territory *t = (*all2)[i];
    Player *owner = (i % 2 == 0) ? cheater : enemy;
    t->setOwner(owner);
    t->setArmyCount(3);
    owner->addTerritory(t);
  }

  // --- toDefend ---
  cout << "\n=== toDefend() ===" << endl;
  vector<Territory *> *defend = cheaterStrategy->toDefend();
  cout << "Defending " << defend->size() << " territories:" << endl;
  for (Territory *t : *defend)
    cout << "  " << t->getName() << " (" << t->getArmyCount() << " armies)"
         << endl;

  // --- toAttack ---
  cout << "\n=== toAttack() ===" << endl;
  vector<Territory *> *attack = cheaterStrategy->toAttack();
  cout << "Targeting " << attack->size() << " enemy territories:" << endl;
  for (Territory *t : *attack)
    cout << "  " << t->getName() << " (owned by "
         << t->getOwner()->getPlayerName() << ")" << endl;

  // --- issueOrder ---
  cout << "\n=== issueOrder() ===" << endl;
  int committedArmies = 0;
  Deck *deck2 = new Deck();
  vector<Player *> *players2 = new vector<Player *>{cheater, enemy};

  bool more =
      cheater->issueOrder(defend, attack, committedArmies, deck2, players2);
  cout << "(issueOrder returned " << (more ? "true" : "false")
       << " — cheater is done in one call)" << endl;

  // --- Results ---
  cout << "\n=== Ownership after issueOrder() ===" << endl;
  cout << cheater->getPlayerName() << " now owns "
       << cheater->getTerritories()->size() << " territories" << endl;
  cout << enemy->getPlayerName() << " now owns "
       << enemy->getTerritories()->size() << " territories" << endl;

  // Cleanup
  delete defend;
  delete attack;
  delete players2;
  delete deck2;
  delete cheater; // also deletes cheaterStrategy via ~Player
  delete enemy;
  delete engine; // ~GameEngine() deletes map — do NOT delete map separately

  cout << "\n=== Cheater driver complete ===" << endl;
}

int main() {

  int choice;
  do {
    cout << "-------------------------------------" << endl;
    cout << "   Part 1: Player Strategy Pattern   " << endl;
    cout << "-------------------------------------" << endl;

    cout << "Select strategy test to run:" << endl;
    cout << "1. Test Benevolent Player" << endl;
    cout << "2. Test Aggressive Player" << endl;
    cout << "3. Test Neutral Player" << endl;
    cout << "4. Test Human Player" << endl;
    cout << "5. Test Cheater Player" << endl;
    cout << "6. Test Change Strategy Dynamically" << endl;
    cout << "7. Quit" << endl;
    cout << "Enter choice: ";

    if (!(cin >> choice)) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cout << "Invalid choice.\n" << endl;
    } else {
      switch (choice) {
      case 1:
        // Run the benevolent strategy demo
        demoBenevolent();
        break;
      case 2:
        demoAggressive();
        break;
      case 3:
        demoNeutral();
        break;
      case 4:
        demoHuman();
        break;
      case 5:
        demoCheater();
        break;
      case 6:
        switchingDemo();
        break;
      case 7:
        return 0;
      default:
        cout << "Invalid choice\n" << endl;
      }
    }
  } while (true);

  return 0;
}
