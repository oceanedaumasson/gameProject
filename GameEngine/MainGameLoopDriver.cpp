#include "GameEngine.h"
#include "../map/map.h"
#include "../Player/Player.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Test 1, gives different amounts of reinforcements based on territories owned and continent bonuses
void testReinforcement() {
    cout << "\n---Test 1: Reinforcement Phase---" << endl;
    cout << "Demonstrates correct army counts including continent bonus" << endl;

    // Load map
    MapLoader loader;
    Map* map = loader.loadMap("../map/maps/europe.map");

    // Create players
    vector<Player*>* players = new vector<Player*>();
    Player* p1 = new Player("Jane");
    Player* p2 = new Player("John");
    players->push_back(p1);
    players->push_back(p2);

    // Distribute territories evenly
    vector<Territory*>* territories = map->getTerritories();
    for (int i = 0; i < territories->size(); i++) {
        territories->at(i)->setArmyCount(5);
        if (i % 2 == 0) {
            p1->addTerritory(territories->at(i));
            territories->at(i)->setOwner(p1);
        } else {
            p2->addTerritory(territories->at(i));
            territories->at(i)->setOwner(p2);
        }
    }
    p1->setAllTerritories(new vector<Territory*>(*map->getTerritories()));
    p2->setAllTerritories(new vector<Territory*>(*map->getTerritories()));

    // Set up engine
    GameEngine* engine = new GameEngine();
    engine->setMap(map);
    engine->setPlayers(players);

    // Case 1: Equal distribution
    cout << "\n---Case 1: Equal distribution (12 territories each)---" << endl;
    cout << "Pool before: Jane = " << *p1->getArmyPool() << ", John = " << *p2->getArmyPool() << endl;
    engine->reinforcementPhase();
    cout << "Pool after:  Jane = " << *p1->getArmyPool() << ", John = " << *p2->getArmyPool() << endl << endl;

    // Reset pools
    *p1->getArmyPool() = 0;
    *p2->getArmyPool() = 0;

    // Case 2: Continent bonus - give Jane first 12 territories (North + East Europe)
    // Clear and reassign territories
    while (!p1->getTerritories()->empty()) p1->removeTerritory(p1->getTerritories()->front());
    while (!p2->getTerritories()->empty()) p2->removeTerritory(p2->getTerritories()->front());

    for (int i = 0; i < territories->size(); i++) {
        if (i < 12) {
            p1->addTerritory(territories->at(i));
            territories->at(i)->setOwner(p1);
        } else {
            p2->addTerritory(territories->at(i));
            territories->at(i)->setOwner(p2);
        }
    }

    cout << "\n---Case 2: Continent bonus---" << endl;
    cout << "Jane owns North+East Europe (bonus: 9), John owns South+West Europe (bonus: 8)" << endl;
    cout << "Pool before: Jane = " << *p1->getArmyPool() << ", John = " << *p2->getArmyPool() << endl;
    engine->reinforcementPhase();
    cout << "Pool after:  Jane = " << *p1->getArmyPool() << ", John = " << *p2->getArmyPool() << endl << endl;

    // Cleanup
    delete engine;
    engine = nullptr;
    map = nullptr;
    p1 = nullptr;
    p2 = nullptr;
    players = nullptr;
}

// Test 2, goes through main game loop once into reinforcement, issue orders and execute orders phases
// Tests deploy, advance and play card options
void testGameLoop() {
    cout << "\n---Test 2: Deploy Enforcement and Advance Orders---" << endl;
    cout << "Players start with armies in pool." << endl;
    cout << "Only deploy prompts appear until pool is empty, then advance/card menu appears." << endl << endl;
    cout << "[Note: Territories start with 5 armies to demonstrate advance orders for demo]" << endl;

     // Load map
    MapLoader loader;
    Map* map = loader.loadMap("../map/maps/europe.map");

    // Create players
    vector<Player*>* players = new vector<Player*>();
    Player* p1 = new Player("Jane");
    Player* p2 = new Player("John");
    players->push_back(p1);
    players->push_back(p2);

    vector<Territory*>* territories = map->getTerritories();
    for (int i = 0; i < territories->size(); i++) {
        territories->at(i)->setArmyCount(5); // Give starting armies so advance orders have something to move for demo
        // Distribute territories evenly
        if (i % 2 == 0) {
            p1->addTerritory(territories->at(i));
            territories->at(i)->setOwner(p1);
        } else {
            p2->addTerritory(territories->at(i));
            territories->at(i)->setOwner(p2);
        }
    }
    
    p1->setAllTerritories(new vector<Territory*>(*map->getTerritories()));
    p2->setAllTerritories(new vector<Territory*>(*map->getTerritories()));

    // Create deck
    Deck* deck = new Deck();
    deck->addCard(new Card(CardType::Bomb));
    deck->addCard(new Card(CardType::Reinforcement));
    deck->addCard(new Card(CardType::Airlift));
    deck->addCard(new Card(CardType::Diplomacy));
    deck->addCard(new Card(CardType::Blockade));
    // Give players cards
    deck->draw(p1->getHand());
    deck->draw(p1->getHand());
    deck->draw(p2->getHand());
    deck->draw(p2->getHand());

    // Set up engine
    GameEngine* engine = new GameEngine();
    engine->setMap(map);
    engine->setPlayers(players);

    // Run only one turn, for demo
    engine->mainGameLoop();

    // Cleanup     
    delete deck;
    deck = nullptr;
    delete engine;
    engine = nullptr;
    deck = nullptr;
    map = nullptr;
    p1 = nullptr;
    p2 = nullptr;
    players = nullptr;

}

// Test 3, player elimination and win condition
// Game immediatly ends when John has no territories, Jane wins
void testPlayerEliminationAndWin() {
    cout << "\n---Test 3: Player Elimination and Win Condition---" << endl;
    cout << "John starts with no territories and is eliminated immediately, Jane wins" << endl;

    // Load map
    MapLoader loader;
    Map* map = loader.loadMap("../map/maps/europe.map");

    // Create players
    vector<Player*>* players = new vector<Player*>();
    Player* p1 = new Player("Jane");
    Player* p2 = new Player("John");
    players->push_back(p1);
    players->push_back(p2);

    // Give all territories to Jane, none to John
    vector<Territory*>* territories = map->getTerritories();
    for (int i = 0; i < territories->size(); i++) {
        territories->at(i)->setArmyCount(5);
        p1->addTerritory(territories->at(i));
        territories->at(i)->setOwner(p1);
    }
    p1->setAllTerritories(map->getTerritories());
    p2->setAllTerritories(map->getTerritories());

    // Set up engine
    GameEngine* engine = new GameEngine();
    engine->setMap(map);
    engine->setPlayers(players);

    engine->mainGameLoop();
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Cleanup
    delete engine;
    engine = nullptr;
    map = nullptr;
    p1 = nullptr;
    p2 = nullptr;
    players = nullptr;
}

int main() {
    int choice;
    do {
        cout << "-----------------------------" << endl;
        cout << "   Part 3: Main Game Loop    " << endl;
        cout << "-----------------------------" << endl;

        cout << "Select test to run:" << endl;
        cout << "1. Test Reinforcement" << endl;
        cout << "2. Test Deploy and Advance" << endl;
        cout << "3. Player elimination and Win Condition" << endl;
        cout << "4. Quit" << endl;
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice.\n" << endl;
            } else {
            switch(choice) {
                case 1:
                    testReinforcement();
                    break;
                case 2:
                    testGameLoop();
                    break;
                case 3:
                    testPlayerEliminationAndWin();
                    break;
                case 4:
                    return 0;
                default:
                    cout << "Invalid choice\n" << endl;
            }
        }
    } while (true);
    return 0;
}