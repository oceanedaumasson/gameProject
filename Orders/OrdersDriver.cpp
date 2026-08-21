/**
 * @file OrdersDriver.cpp
 * @brief Assignment 1 : Orders class driver
 * @author Cuneyt YILDIRIM
 * @version 1.0
 *
 */
#include "../Player/Player.h"
#include "../cards/Cards.h"
#include "../map/map.h"
#include "Orders.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <vector>
using std::cout;
using std::endl;

int main() {
  cout << "\n=============================================" << endl;
  cout << "==== Orders Class Functionality Testing =====" << endl;
  cout << "=============================================\n" << endl;

  // ---- Setup: Players and Territories ----
  Player *arwen = new Player("Arwen");
  Player *aragorn = new Player("Aragorn");

  Territory *quebec = new Territory(1, "Quebec", 1);
  Territory *ontario = new Territory(2, "Ontario", 1);
  Territory *alberta = new Territory(3, "Alberta", 1);

  quebec->setArmyCount(10);
  ontario->setArmyCount(7);
  alberta->setArmyCount(5);

  // Arwen owns Quebec and Ontario
  arwen->addTerritory(quebec);
  arwen->addTerritory(ontario);
  quebec->setOwner(arwen);
  ontario->setOwner(arwen);

  // Aragorn owns Alberta
  aragorn->addTerritory(alberta);
  alberta->setOwner(aragorn);

  // Extra territory for Arwen used by Blockade (keeps Ontario available for
  // Airlift)
  Territory *manitoba = new Territory(4, "Manitoba", 1);
  manitoba->setArmyCount(4);
  arwen->addTerritory(manitoba);
  manitoba->setOwner(arwen);

  // Give Arwen an army pool to deploy from
  arwen->setArmyPool(new int(15));

  // ---- Create all 6 order types ----
  cout << "---- 1. Creating all 6 order types ----\n" << endl;

  Order *deployOrder = new Deploy(arwen, quebec, new int(3));
  Order *advanceOrder =
      new Advance(arwen, quebec, ontario, nullptr, new int(2));
  Order *bombOrder = new Bomb(aragorn, quebec);
  Order *blockadeOrder = new Blockade(arwen, manitoba);
  Order *airliftOrder = new Airlift(arwen, ontario, quebec, new int(3));
  Order *negotiateOrder = new Negotiate(arwen, aragorn);

  // Adjacency for Advance: Quebec -> Ontario
  auto *adjAdv = new unordered_map<Territory *, vector<Territory *>>();
  (*adjAdv)[quebec] = {ontario};
  advanceOrder->setAdjacencyList(adjAdv);

  // Adjacency for Bomb: Alberta (Aragorn) -> Quebec
  auto *adjBomb = new unordered_map<Territory *, vector<Territory *>>();
  (*adjBomb)[alberta] = {quebec};
  bombOrder->setAdjacencyList(adjBomb);

  cout << "Created: Deploy, Advance, Bomb, Blockade, Airlift, Negotiate\n"
       << endl;

  // ---- Add orders to OrdersList ----
  cout << "---- 2. Adding orders to OrdersList ----\n" << endl;

  OrdersList *orders = new OrdersList();
  orders->addOrder(deployOrder);
  orders->addOrder(advanceOrder);
  orders->addOrder(bombOrder);
  orders->addOrder(blockadeOrder);
  orders->addOrder(airliftOrder);
  orders->addOrder(negotiateOrder);

  cout << "Total orders in list: " << orders->getSize() << endl;
  cout << *orders << endl;

  // Valid Orders
  // ---- Validate and Execute orders ----
  cout << "---- 3. Validating and Executing orders ----\n" << endl;

  cout << ">> Executing Deploy (Arwen-> Quebec, 3 armies):" << endl;
  deployOrder->execute();
  cout << *deployOrder << "\n" << endl;

  cout << ">> Executing Advance (Arwen: Quebec -> Ontario, 2 armies):" << endl;
  advanceOrder->execute();
  cout << *advanceOrder << "\n" << endl;

  cout << ">> Executing Bomb (Aragorn-> Quebec, 4 armies):" << endl;
  bombOrder->execute();
  cout << *bombOrder << "\n" << endl;

  cout << ">> Executing Blockade (Arwen-> Manitoba, 4 armies):" << endl;
  blockadeOrder->execute();
  cout << *blockadeOrder << "\n" << endl;

  cout << ">> Executing Airlift (Arwen: Ontario -> Quebec, 3 armies):" << endl;
  airliftOrder->execute();
  cout << *airliftOrder << "\n" << endl;

  cout << ">> Executing Negotiate (Arwen<-> Aragorn):" << endl;
  negotiateOrder->execute();
  cout << *negotiateOrder << "\n" << endl;

  // =====================================================================
  // SECTION 4: Invalid Order Validation
  // =====================================================================
  cout << "---- 4. Invalid Order Validation ----\n" << endl;

  // Invalid Deploy: target territory not owned by issuing player
  Deploy *inv_deploy1 = new Deploy(arwen, alberta, new int(3));
  cout << ">> Deploy to enemy territory (Alberta not owned by Arwen):" << endl;
  if (!inv_deploy1->validate())
    cout << "   [INVALID] " << *inv_deploy1->getEffect() << "\n" << endl;
  delete inv_deploy1;

  // Invalid Deploy: requesting more armies than in pool (arwen pool = 12)
  Deploy *inv_deploy2 = new Deploy(arwen, quebec, new int(999));
  cout << ">> Deploy 999 armies (Arwen pool = 12):" << endl;
  if (!inv_deploy2->validate())
    cout << "   [INVALID] " << *inv_deploy2->getEffect() << "\n" << endl;
  delete inv_deploy2;

  // Invalid Bomb: targeting own territory
  Bomb *inv_bomb = new Bomb(aragorn, alberta);
  cout << ">> Bomb own territory (Aragorn bombs Alberta):" << endl;
  if (!inv_bomb->validate())
    cout << "   [INVALID] " << *inv_bomb->getEffect() << "\n" << endl;
  delete inv_bomb;

  // Invalid Blockade: target territory belongs to an enemy
  Blockade *inv_blockade = new Blockade(arwen, alberta);
  cout << ">> Blockade enemy territory (Alberta not owned by Arwen):" << endl;
  if (!inv_blockade->validate())
    cout << "   [INVALID] " << *inv_blockade->getEffect() << "\n" << endl;
  delete inv_blockade;

  // Invalid Airlift: source territory not owned by player
  Airlift *inv_airlift = new Airlift(arwen, alberta, quebec, new int(2));
  cout << ">> Airlift from enemy territory (Alberta not owned by Arwen):"
       << endl;
  if (!inv_airlift->validate())
    cout << "   [INVALID] " << *inv_airlift->getEffect() << "\n" << endl;
  delete inv_airlift;

  // Invalid Negotiate: player targeting themselves
  Negotiate *inv_neg = new Negotiate(arwen, arwen);
  cout << ">> Negotiate with self (Arwen targets Arwen):" << endl;
  if (!inv_neg->validate())
    cout << "   [INVALID] " << *inv_neg->getEffect() << "\n" << endl;
  delete inv_neg;

  // =====================================================================
  // SECTION 5: Territory Conquest via Advance + Card on Conquest
  // =====================================================================
  cout << "---- 5. Territory Conquest via Advance + Card on Conquest ----\n"
       << endl;

  srand(42); // fixed seed for reproducibility

  Player *legolas = new Player("Legolas");
  Player *sauron = new Player("Sauron");

  Territory *shire = new Territory(10, "Shire", 1);
  Territory *mordor = new Territory(11, "Mordor", 1);

  shire->setArmyCount(30);
  mordor->setArmyCount(1); // Overwhelmingly outnumbered
  shire->setOwner(legolas);
  mordor->setOwner(sauron);
  legolas->addTerritory(shire);
  sauron->addTerritory(mordor);
  legolas->setArmyPool(new int(50));

  // Deck for card reward
  Deck *deck = new Deck();
  deck->addCard(new Card(CardType::Bomb));
  deck->addCard(new Card(CardType::Airlift));
  deck->addCard(new Card(CardType::Diplomacy));

  // Build adjacency: Shire -> Mordor
  auto *adjConquest = new unordered_map<Territory *, vector<Territory *>>();
  (*adjConquest)[shire] = {mordor};

  Advance *conquestOrder =
      new Advance(legolas, shire, mordor, deck, new int(25));
  conquestOrder->setAdjacencyList(adjConquest);

  cout << "Before attack:" << endl;
  cout << "  Shire armies  : " << shire->getArmyCount()
       << "  Owner: " << shire->getOwner()->getPlayerName() << endl;
  cout << "  Mordor armies : " << mordor->getArmyCount()
       << "  Owner: " << mordor->getOwner()->getPlayerName() << endl;
  cout << "  Legolas hand  : " << legolas->getHand()->size() << " cards"
       << endl;

  conquestOrder->execute();

  cout << "\nAfter Advance (Legolas sends 25 armies against 1 defender):"
       << endl;
  cout << "  Effect  : " << *conquestOrder->getEffect() << endl;
  cout << "  Mordor owner  : " << mordor->getOwner()->getPlayerName() << endl;
  cout << "  Mordor armies : " << mordor->getArmyCount() << endl;
  cout << "  Shire armies  : " << shire->getArmyCount() << endl;
  cout << "  Legolas hand  : " << legolas->getHand()->size()
       << " cards (card drawn on conquest)" << endl;

  // Verify no second card is issued in the same turn
  auto *adj2ndAttack = new unordered_map<Territory *, vector<Territory *>>();
  Territory *anotherTerr = new Territory(12, "Rohan", 1);
  Player *rohanOwner = new Player("Theoden");
  anotherTerr->setArmyCount(1);
  anotherTerr->setOwner(rohanOwner);
  rohanOwner->addTerritory(anotherTerr);
  (*adj2ndAttack)[mordor] = {anotherTerr};

  Advance *secondConquest =
      new Advance(legolas, mordor, anotherTerr, nullptr, new int(10));
  secondConquest->setAdjacencyList(adj2ndAttack);
  secondConquest->execute();

  cout << "\nAfter second conquest attempt in same turn:" << endl;
  cout << "  Effect : " << *secondConquest->getEffect() << endl;
  cout << "  Legolas hand : " << legolas->getHand()->size()
       << " cards (no additional card despite second conquest)" << endl;

  // =====================================================================
  // SECTION 6: Negotiate Prevents Attacks Between Players
  // =====================================================================
  cout << "\n---- 6. Negotiate Order Prevents Attacks ----\n" << endl;

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

  // Issue and execute Negotiate
  Negotiate *truce = new Negotiate(elvish, orcish);
  cout << ">> Executing Negotiate (Elvish <-> Orcish):" << endl;
  truce->execute();
  cout << "   Effect: " << *truce->getEffect() << "\n" << endl;

  // Elvish tries to attack Orcish
  auto *adjElvish = new unordered_map<Territory *, vector<Territory *>>();
  (*adjElvish)[rivendell] = {isengard};
  Advance *blockedAttack1 =
      new Advance(elvish, rivendell, isengard, nullptr, new int(8));
  blockedAttack1->setAdjacencyList(adjElvish);

  cout << ">> Elvish attempts to Advance on Isengard (Orcish) after Negotiate:"
       << endl;
  blockedAttack1->execute();
  cout << "   Effect: " << *blockedAttack1->getEffect() << endl;
  cout << "   Isengard owner: " << isengard->getOwner()->getPlayerName()
       << " (unchanged)" << "\n"
       << endl;

  // Orcish tries to attack Elvish
  auto *adjOrcish = new unordered_map<Territory *, vector<Territory *>>();
  (*adjOrcish)[isengard] = {rivendell};
  Advance *blockedAttack2 =
      new Advance(orcish, isengard, rivendell, nullptr, new int(5));
  blockedAttack2->setAdjacencyList(adjOrcish);

  cout << ">> Orcish attempts to Advance on Rivendell (Elvish) after Negotiate:"
       << endl;
  blockedAttack2->execute();
  cout << "   Effect: " << *blockedAttack2->getEffect() << endl;
  cout << "   Rivendell owner: " << rivendell->getOwner()->getPlayerName()
       << " (unchanged)" << "\n"
       << endl;

  // =====================================================================
  // SECTION 7: Blockade Transfers Territory to Neutral Player
  // =====================================================================
  cout << "---- 7. Blockade Transfers Ownership to Neutral Player ----\n"
       << endl;

  Player *dunedain = new Player("Dunedain");
  Territory *helmsDeep = new Territory(30, "Helms Deep", 3);
  helmsDeep->setArmyCount(8);
  helmsDeep->setOwner(dunedain);
  dunedain->addTerritory(helmsDeep);

  cout << "Before Blockade:" << endl;
  cout << "  Helms Deep armies: " << helmsDeep->getArmyCount()
       << "  Owner: " << helmsDeep->getOwner()->getPlayerName() << endl;

  Blockade *blockadeOrder2 = new Blockade(dunedain, helmsDeep);
  blockadeOrder2->execute();

  cout << "After Blockade:" << endl;
  cout << "  Effect: " << *blockadeOrder2->getEffect() << endl;
  cout << "  Helms Deep armies: " << helmsDeep->getArmyCount() << " (doubled)"
       << endl;
  cout << "  Helms Deep owner : " << helmsDeep->getOwner()->getPlayerName()
       << " (transferred to Neutral)\n"
       << endl;

  // =====================================================================
  // SECTION 8: All Orders Issued via Player::issueOrder()
  // =====================================================================
  cout << "---- 8. All 6 Orders Issued via Player::issueOrder() ----\n" << endl;

  Player *commander = new Player("Commander");
  vector<Player::OrderType> orderTypes = {
      Player::OrderType::EDeploy,  Player::OrderType::EAdvance,
      Player::OrderType::EBomb,    Player::OrderType::EBlockade,
      Player::OrderType::EAirlift, Player::OrderType::ENegotiate};
  for (auto type : orderTypes) {
    Order *order = commander->createOrder(type);
    commander->getOrderList()->addOrder(order);
    cout << "Player " << commander->getPlayerName()
         << " issued an order of type " << commander->enumToString(type)
         << " and total order count is " << commander->getOrderList()->getSize()
         << endl;
  }
  cout << "\nCommander's OrdersList (" << commander->getOrderList()->getSize()
       << " orders):\n"
       << *commander->getOrderList() << endl;

  // ---- Cleanup ----
  cout << "---- Cleanup ----" << endl;

  // Section 1-3 objects (orders vector owns the order pointers)
  delete orders;
  delete arwen;
  delete aragorn;
  delete quebec;
  delete ontario;
  delete alberta;
  delete manitoba;

  // Section 5 objects
  delete conquestOrder;  // owns its armiesNum + adjacencyList (adjConquest)
  delete secondConquest; // owns its armiesNum + adjacencyList (adj2ndAttack)
  delete deck;
  delete legolas;
  delete sauron;
  delete shire;
  delete mordor;
  delete anotherTerr;
  delete rohanOwner;

  // Section 6 objects
  delete truce;
  delete blockedAttack1; // owns adjElvish
  delete blockedAttack2; // owns adjOrcish
  delete elvish;
  delete orcish;
  delete rivendell;
  delete isengard;

  // Section 7 objects (Blockade internally creates a Neutral Player)
  delete blockadeOrder2;
  delete dunedain;
  delete helmsDeep;

  // Section 8
  delete commander;

  cout << "\n=============================================" << endl;
  cout << "==== Orders Class Testing Complete ==========" << endl;
  cout << "=============================================\n" << endl;

  return 0;
}
