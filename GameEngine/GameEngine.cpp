/**
 * @file GameEngine.cpp
 * @brief Implementation of the GameEngine and State classes.
 * Handles the game flow, state transitions, and user commands.
 */

#include "GameEngine.h"
#include "../CommandProcessor/CommandProcessing.h"
#include "../map/map.h"
#include "../Player/Player.h"
#include "../cards/Cards.h"
#include "../PlayerStrategy/PlayerStrategies.h"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

// == Game Engine logic ==

// Constructor
GameEngine::GameEngine()
{
	currentState = new StartState(); // Start game in the StartState
	gameMap = nullptr;
	gamePlayers = new vector<Player *>();
	gameDeck = new Deck();
	commandProcessor = new CommandProcessor(this);
}

// Destructor
GameEngine::~GameEngine()
{
	delete currentState;
	delete gameMap;
	delete gameDeck;
	delete commandProcessor;
	if (gamePlayers != nullptr)
	{
		for (Player *p : *gamePlayers)
		{
			delete p;
		}
		delete gamePlayers;
	}
}

// Copy Constructor
GameEngine::GameEngine(const GameEngine &other)
{
	// Manually create new state of the same type
	string stateName = other.currentState->getName();

	if (stateName == "Start")
		currentState = new StartState();
	else if (stateName == "Map Loaded")
		currentState = new MapLoadedState();
	else if (stateName == "Map Validated")
		currentState = new MapValidatedState();
	else if (stateName == "Players Added")
		currentState = new PlayersAddedState();
	else if (stateName == "Assign Reinforcement")
		currentState = new AssignReinforcementState();
	else if (stateName == "Issue Orders")
		currentState = new IssueOrdersState();
	else if (stateName == "Execute Orders")
		currentState = new ExecuteOrdersState();
	else if (stateName == "Win")
		currentState = new WinState();
	else
		currentState = new StartState(); // Fallback
}

// Assignment Operator
GameEngine &GameEngine::operator=(const GameEngine &other)
{
	if (this != &other)
	{
		delete currentState; // Delete old state

		// Copy logic (same as above)
		string stateName = other.currentState->getName();
		if (stateName == "Start")
			currentState = new StartState();
		else if (stateName == "Map Loaded")
			currentState = new MapLoadedState();
		else if (stateName == "Map Validated")
			currentState = new MapValidatedState();
		else if (stateName == "Players Added")
			currentState = new PlayersAddedState();
		else if (stateName == "Assign Reinforcement")
			currentState = new AssignReinforcementState();
		else if (stateName == "Issue Orders")
			currentState = new IssueOrdersState();
		else if (stateName == "Execute Orders")
			currentState = new ExecuteOrdersState();
		else if (stateName == "Win")
			currentState = new WinState();
		else
			currentState = new StartState();
	}
	return *this;
}

// Stream Insertion Operator
ostream &operator<<(ostream &out, const GameEngine &ge)
{
	out << "GameEngine Current State: " << ge.currentState->getName();
	return out;
}

/// @brief gives the new game state as a string
/// @return string of gameEngine effect
string GameEngine::stringToLog() const
{
	return "Game Engine new state: " + currentState->getName();
}

void GameEngine::transition(State *newState)
{
	if (currentState != nullptr)
	{
		delete currentState;
	}
	currentState = newState;
	Notify(this);
	cout << "Transition to -----> " << currentState->getName() << " state" << endl;
}

void GameEngine::handleCommand(string command)
{
	currentState->updateState(this, command);
}

State *GameEngine::getCurrentState()
{
	return currentState;
}

void GameEngine::startupPhase()
{
	cout << "\nGame Startup Phase" << endl;
	bool isStartupComplete = false;

	while (!isStartupComplete)
	{
		cout << "\n[" << currentState->getName() << "] Waiting for command..." << endl;

		// Check if the buffer is empty or doesn't have a newline waiting
		if (cin.peek() == EOF || cin.peek() != '\n')
		{
			// The buffer is empty, so add newline for command processor
			cin.putback('\n');
		}

		commandProcessor->getCommand();
		Command *cmd = commandProcessor->getLatestCommand();

		if (cmd == nullptr)
			continue; // Safety

		string effect = cmd->getCommandEffect();
		if (effect.find("Error") == 0)
			continue; // Skip execution if the processor marked it as an error

		string fullCommand = cmd->getCommandName();
		stringstream ss(fullCommand);
		string commandStr, argumentStr;
		ss >> commandStr >> argumentStr;

		// Load map
		if (commandStr == "loadmap")
		{
			MapLoader loader;
			Map *loadedMap = loader.loadMap(argumentStr);

			if (loadedMap != nullptr)
			{
				if (gameMap != nullptr)
					delete gameMap;
				gameMap = loadedMap;
				this->transition(new MapLoadedState());
				cout << "Success: Map " << argumentStr << " loaded." << endl;
			}
			else
			{
				cout << "Error: Could not load the map file '" << argumentStr << "'" << endl;
			}
		}
		// Validate map
		else if (commandStr == "validatemap")
		{
			if (gameMap != nullptr && gameMap->validate())
			{
				this->transition(new MapValidatedState());
				cout << "Success: Map is valid" << endl;
			}
			else
			{
				cout << "Error: Map is invalid" << endl;
			}
		}
		// Add Player
		else if (commandStr == "addplayer")
		{
			if (gamePlayers->size() >= 6)
			{
				cout << "Error: Maximum of 6 players reached." << endl;
			}
			else
			{
				gamePlayers->push_back(new Player(argumentStr));
				this->transition(new PlayersAddedState());
				cout << "Success: Player " << argumentStr << " added." << endl;
			}
		}
		// Game Start
		else if (commandStr == "gamestart")
		{
			if (gamePlayers->size() < 2)
			{
				cout << "Error: Need at least 2 players to start." << endl;
			}
			else
			{
				cout << "\nExecuting Game Start..." << endl;

				// Fairly distribute all territories
				vector<Territory *> *allTerritories = gameMap->getTerritories();
				auto rd = std::random_device{};
				auto g = std::default_random_engine{rd()};
				std::shuffle(allTerritories->begin(), allTerritories->end(), g);

				int playerIndex = 0;
				for (Territory *t : *allTerritories)
				{
					Player *currentP = (*gamePlayers)[playerIndex];
					t->setOwner(currentP);
					currentP->addTerritory(t);
					playerIndex = (playerIndex + 1) % gamePlayers->size();
				}
				cout << "Territories distributed among " << gamePlayers->size() << " players." << endl;

				// Randomly determine order of play
				std::shuffle(gamePlayers->begin(), gamePlayers->end(), g);
				cout << "Play order randomized. Player 1 is: " << (*gamePlayers)[0]->getPlayerName() << endl;

				// Armies and Cards
				for (Player *p : *gamePlayers)
				{
					p->setArmyPool(new int(50));
					if (p->getHand() == nullptr)
						p->setHand(new Hand());
					gameDeck->draw(p->getHand());
					gameDeck->draw(p->getHand());
					cout << " > Player " << p->getPlayerName() << ": 50 armies assigned, 2 cards drawn." << endl;
				}

				this->transition(new AssignReinforcementState());

				// Startup phase driver logs: play order
				cout << "\nFINAL PLAY ORDER:" << endl;
				for (size_t i = 0; i < gamePlayers->size(); ++i)
				{
					cout << "  " << (i + 1) << ". " << (*gamePlayers)[i]->getPlayerName() << endl;
				}

				// Startup phase dirver logs: player status
				for (auto *p : *gamePlayers)
				{
					cout << "PLAYER: " << p->getPlayerName() << endl;

					// Check armies
					if (p->getArmyPool() != nullptr)
					{
						cout << "  - Armies in Pool: " << *(p->getArmyPool()) << endl;
					}

					// Check cards
					if (p->getHand() != nullptr)
					{
						cout << "  - Cards Played: " << p->getHand()->size() << " (Remaining: 2)" << endl;
					}

					// Check territories
					if (p->getTerritories() != nullptr)
					{
						cout << "  - Territories Owned: " << p->getTerritories()->size() << endl;
						for (auto *t : *(p->getTerritories()))
						{
							cout << "      > " << t->getName() << " (ID: " << t->getId() << ")" << endl;
						}
					}
				}

				isStartupComplete = true;
			}
		}
	}
}

// Sets the game map
void GameEngine::setMap(Map *map)
{
	gameMap = map;
}

// Sets the list of players
void GameEngine::setPlayers(std::vector<Player *> *players)
{
	gamePlayers = players;
}

// Returns the game map
Map *GameEngine::getMap() const
{
	return gameMap;
}

// Returns the list of players
std::vector<Player *> *GameEngine::getPlayers() const
{
	return gamePlayers;
}

// Main game loop: Loops through reinforcement, issue orders and execute orders phases until one player wins
void GameEngine::mainGameLoop()
{
	int turn = 1;
	while (true)
	{
		transition(new AssignReinforcementState());
		reinforcementPhase();

		// Remove players with no territories before starting turn
		vector<Player *> remaining;
		for (Player *p : *gamePlayers)
		{
			if (p->getTerritories()->empty())
			{
				cout << p->getPlayerName() << " has been eliminated." << endl;
			}
			else
			{
				remaining.push_back(p);
			}
		}
		*gamePlayers = remaining;

		// Check win condition
		if (gamePlayers->size() == 1)
		{
			transition(new WinState());
			cout << (*gamePlayers)[0]->getPlayerName() << " wins the game!" << endl;
			return;
		}

		transition(new IssueOrdersState());
		issueOrdersPhase();

		transition(new ExecuteOrdersState());
		executeOrdersPhase();

		// Remove players who have no more territories
		remaining.clear();
		for (Player *p : *gamePlayers)
		{
			if (p->getTerritories()->empty())
			{
				cout << p->getPlayerName() << " has been eliminated." << endl;
			}
			else
			{
				remaining.push_back(p);
			}
		}
		*gamePlayers = remaining;

		// Transition to win state if only one player remains
		if (gamePlayers->size() == 1)
		{
			transition(new WinState());
			cout << (*gamePlayers)[0]->getPlayerName() << " wins the game!" << endl;
			return;
		}

		if (turn >= 1)
		{
			cout << "\nDemo ends after 1 turn...\n"
				 << endl;
			break;
		}
	}
}

// Reinforcement phase: give armies to each players based on territories owned
void GameEngine::reinforcementPhase()
{
	cout << "\n-----------------------------" << endl;
	cout << "   Reinforcement Phase" << endl;
	cout << "-----------------------------" << endl;

	for (Player *player : *gamePlayers)
	{
		// Armies given are number of territories divided by 3, or 3 minimum
		int numTerritories = player->getTerritories()->size();
		int armies = max(3, numTerritories / 3);

		// Get all continent ids from the map into one list
		vector<Territory *> *allTerritories = gameMap->getTerritories();
		vector<int> continentIds;
		for (Territory *t : *allTerritories)
		{
			int contID = t->getContinentId();
			bool found = false;
			for (int id : continentIds)
			{
				if (id == contID)
				{
					found = true;
					break;
				}
			}
			// Add all continent ids to list once
			if (!found)
			{
				continentIds.push_back(contID);
			}
		}

		// Check if player owns all terrritories in each continent
		for (int contID : continentIds)
		{
			bool ownsAll = true;
			for (Territory *t : *allTerritories)
			{
				if (t->getContinentId() == contID && t->getOwner() != player)
				{
					ownsAll = false;
					break;
				}
			}
			// Give continent bonus if player owns all territories in the continent
			if (ownsAll)
			{
				Continent *c = gameMap->getContinentById(contID);
				armies += c->getArmyBonusCount();
			}
		}

		// Add armies to player's army pool
		*player->getArmyPool() += armies;
		cout << player->getPlayerName() << " receives " << armies << " armies. Pool: " << *player->getArmyPool() << endl;
	}
}

// Issue orders phase: Players issue orders in round-robin fashion
void GameEngine::issueOrdersPhase()
{
	cout << "\n-----------------------------" << endl;
	cout << "   Issuing Orders Phase" << endl;
	cout << "-----------------------------" << endl;
	int n = gamePlayers->size();

	// Each player sets their priority lists once at start of turn
	vector<vector<Territory *> *> defendLists(n);
	vector<vector<Territory *> *> attackLists(n);
	for (int i = 0; i < n; i++)
	{
		cout << "------" << gamePlayers->at(i)->getPlayerName() << " sets priority lists" << "------" << endl;
		if (i > 0)
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		defendLists[i] = gamePlayers->at(i)->toDefend();
		cout << endl;
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		attackLists[i] = gamePlayers->at(i)->toAttack();
		cout << endl;
	}
	vector<bool> done(n, false);
	bool allDone = false;

	vector<int> committedArmies(n, 0);
	while (!allDone)
	{ // Keep looping until all players are done
		allDone = true;
		for (int i = 0; i < n; i++)
		{
			if (!done[i])
			{
				cout << "\n------ Player " << gamePlayers->at(i)->getPlayerName() << " issuing order" << "------" << endl;
				done[i] = !gamePlayers->at(i)->issueOrder(defendLists[i], attackLists[i], committedArmies[i], gameDeck, gamePlayers);
				if (!done[i])
					allDone = false;
			}
		}
	}

	// Cleanup
	for (int i = 0; i < n; i++)
	{
		delete defendLists[i];
		defendLists[i] = nullptr;
		delete attackLists[i];
		attackLists[i] = nullptr;
	}
}

// Execute orders phase: Executes all player orders in round-robin fashion
// Executing all deploy orders before other order types
void GameEngine::executeOrdersPhase()
{
	cout << "\n-----------------------------" << endl;
	cout << "   Execute Orders Phase" << endl;
	cout << "-----------------------------" << endl;

	// First execute all deploy orders in round-robin
	bool hasDeployOrders;
	do
	{
		hasDeployOrders = false;
		for (Player *player : *gamePlayers)
		{
			vector<Order *> orders = player->getOrderList()->getOrders();
			for (int i = 0; i < orders.size(); i++)
			{
				if (*orders[i]->getOrderType() == "Deploy")
				{

					Deploy *d = dynamic_cast<Deploy *>(orders[i]);
					if (d)
						*d->getPlayer()->getArmyPool() += *d->getArmiesNum();

					orders[i]->execute();
					cout << "[" << player->getPlayerName() << "] "
						 << *orders[i]->getEffect() << endl;
					player->getOrderList()->removeOrder(i);
					hasDeployOrders = true; // Keep looking once a deploy order is found
					break;
				}
			}
		}
		// Keep looping until all deploy orders are executed
	} while (hasDeployOrders);

	// Then execute all remaining orders in round-robin
	bool hasOrders;
	do
	{
		hasOrders = false;
		for (Player *player : *gamePlayers)
		{
			// Execute first order in list each time
			if (player->getOrderList()->getSize() > 0)
			{
				Order *order = player->getOrderList()->getOrders()[0];

				// Check if order is an advance order
				Advance *adv = dynamic_cast<Advance *>(order);
				if (adv != nullptr)
				{
					// Skip execution if territories aren't adjacent
					if (!gameMap->checkAdjacent(adv->getSourceTerritory(), adv->getTargetTerritory()))
					{
						cout << "[" << player->getPlayerName() << "] Advance invalid: territories not adjacent." << endl;
						player->getOrderList()->removeOrder(0);
						continue;
					}
					// Get adjacency list for advance order
					// Map stores vector<Territory*>* but Order expects vector<Territory*>
					const unordered_map<Territory *, vector<Territory *> *> *rawAdj = gameMap->getAdjacencyList();
					unordered_map<Territory *, vector<Territory *>> *adjList = new unordered_map<Territory *, vector<Territory *>>();
					for (auto it = rawAdj->begin(); it != rawAdj->end(); ++it)
					{
						vector<Territory *> neighbors;
						for (Territory *t : *it->second)
						{
							neighbors.push_back(t);
						}
						(*adjList)[it->first] = neighbors;
					}

					// Delete old adjacency list before setting new one to avoid memory leak
					delete adv->getAdjacencyList();
					adv->setAdjacencyList(adjList);

					Player* attacker = adv->getPlayer();
					Player* defender = adv->getTargetTerritory()->getOwner();

					// Check if its an attack order
					if (attacker != defender && defender != nullptr) {
						if (dynamic_cast<NeutralPlayerStrategy*>(defender->getPlayerStrategy()) != nullptr) {
							cout << defender->getPlayerName() << " was attacked and becomes AGGRESSIVE!" << endl;

							// Change player strategy from neutral to aggressive is neutral player is attacked
							PlayerStrategy* oldStrategy = defender->getPlayerStrategy();
							defender->setPlayerStrategy(new AggressivePlayerStrategy(defender, gameMap));
							delete oldStrategy;
						}
					}
				}

				order->execute();
				cout << "  [" << player->getPlayerName() << "] "
					 << *order->getEffect() << endl;
				player->getOrderList()->removeOrder(0);
				hasOrders = true; // Keep looking once an order is found
			}
		}
		// Keep looping until all orders are executed
	} while (hasOrders);
}

// == Game States ==

// State abstract base class

State::State() {}
State::~State() {}
State::State(const State &other) {}							  // Nothing to copy yet
State &State::operator=(const State &other) { return *this; } // Nothing to assign yet

std::ostream &operator<<(std::ostream &out, const State &s)
{ // Smart printer uses the virtual getName()
	out << s.getName();
	return out;
}

// Start State

StartState::StartState() {}
StartState::StartState(const StartState &other) : State(other) {}

StartState &StartState::operator=(const StartState &other)
{
	State::operator=(other);
	return *this;
}

std::ostream &operator<<(std::ostream &out, const StartState &s)
{
	return out << s.getName();
}

void StartState::updateState(GameEngine *ge, string command)
{
	if (command == "loadmap")
	{
		ge->transition(new MapLoadedState());
	}
	else
	{
		cout << "Error: invalid command" << endl;
	}
}

// Map Loaded State

MapLoadedState::MapLoadedState() {}
MapLoadedState::MapLoadedState(const MapLoadedState &other) : State(other) {}

MapLoadedState &MapLoadedState::operator=(const MapLoadedState &other)
{
	State::operator=(other);
	return *this;
}

std::ostream &operator<<(std::ostream &out, const MapLoadedState &s)
{
	return out << s.getName();
}

void MapLoadedState::updateState(GameEngine *ge, string command)
{
	if (command == "loadmap")
	{
		ge->transition(new MapLoadedState());
	}
	else if (command == "validatemap")
	{
		ge->transition(new MapValidatedState());
	}
	else
	{
		cout << "Error: invalid command" << endl;
	}
}

// Map Validated State

MapValidatedState::MapValidatedState() {}
MapValidatedState::MapValidatedState(const MapValidatedState &other) : State(other) {}

MapValidatedState &MapValidatedState::operator=(const MapValidatedState &other)
{
	State::operator=(other);
	return *this;
}

std::ostream &operator<<(std::ostream &out, const MapValidatedState &s)
{
	return out << s.getName();
}

void MapValidatedState::updateState(GameEngine *ge, string command)
{
	if (command == "addplayer")
	{
		ge->transition(new PlayersAddedState());
	}
	else
	{
		cout << "Error: invalid command" << endl;
	}
}

// Players Added State

PlayersAddedState::PlayersAddedState() {}
PlayersAddedState::PlayersAddedState(const PlayersAddedState &other) : State(other) {}

PlayersAddedState &PlayersAddedState::operator=(const PlayersAddedState &other)
{
	State::operator=(other);
	return *this;
}

std::ostream &operator<<(std::ostream &out, const PlayersAddedState &s)
{
	return out << s.getName();
}

void PlayersAddedState::updateState(GameEngine *ge, string command)
{
	if (command == "addplayer")
	{
		ge->transition(new PlayersAddedState());
	}
	else if (command == "assigncountries")
	{
		ge->transition(new AssignReinforcementState());
	}
	else
	{
		cout << "Error: invalid command" << endl;
	}
}

// Assign Reinforcement State

AssignReinforcementState::AssignReinforcementState() {}
AssignReinforcementState::AssignReinforcementState(const AssignReinforcementState &other) : State(other) {}

AssignReinforcementState &AssignReinforcementState::operator=(const AssignReinforcementState &other)
{
	State::operator=(other);
	return *this;
}

std::ostream &operator<<(std::ostream &out, const AssignReinforcementState &s)
{
	return out << s.getName();
}

void AssignReinforcementState::updateState(GameEngine *ge, string command)
{
	if (command == "issueorder")
	{
		ge->transition(new IssueOrdersState());
	}
	else
	{
		cout << "Error: invalid command" << endl;
	}
}

// Issue Orders State

IssueOrdersState::IssueOrdersState() {}
IssueOrdersState::IssueOrdersState(const IssueOrdersState &other) : State(other) {}

IssueOrdersState &IssueOrdersState::operator=(const IssueOrdersState &other)
{
	State::operator=(other);
	return *this;
}

std::ostream &operator<<(std::ostream &out, const IssueOrdersState &s)
{
	return out << s.getName();
}

void IssueOrdersState::updateState(GameEngine *ge, string command)
{
	if (command == "issueorder")
	{
		ge->transition(new IssueOrdersState());
	}
	else if (command == "endissueorders")
	{
		ge->transition(new ExecuteOrdersState());
	}
	else
	{
		cout << "Error: invalid command" << endl;
	}
}

// Execute Orders State

ExecuteOrdersState::ExecuteOrdersState() {}
ExecuteOrdersState::ExecuteOrdersState(const ExecuteOrdersState &other) : State(other) {}

ExecuteOrdersState &ExecuteOrdersState::operator=(const ExecuteOrdersState &other)
{
	State::operator=(other);
	return *this;
}

std::ostream &operator<<(std::ostream &out, const ExecuteOrdersState &s)
{
	return out << s.getName();
}

void ExecuteOrdersState::updateState(GameEngine *ge, string command)
{
	if (command == "execorder")
	{
		ge->transition(new ExecuteOrdersState());
	}
	else if (command == "endexecorders")
	{
		ge->transition(new AssignReinforcementState());
	}
	else if (command == "win")
	{
		ge->transition(new WinState());
	}
	else
	{
		cout << "Error: invalid command" << endl;
	}
}

// Win State

WinState::WinState() {}
WinState::WinState(const WinState &other) : State(other) {}

WinState &WinState::operator=(const WinState &other)
{
	State::operator=(other);
	return *this;
}

std::ostream &operator<<(std::ostream &out, const WinState &s)
{
	return out << s.getName();
}

void WinState::updateState(GameEngine *ge, string command)
{
	if (command == "play")
	{
		ge->transition(new StartState()); // Restart game
	}
	else if (command == "end")
	{
		cout << "Game Over. Type 'quit' to exit." << endl; // Driver handles actual exit
	}
	else
	{
		cout << "Error: invalid command" << endl;
	}
}

// == Helper functions ==

string StartState::getName() const
{
	return "Start";
}

string MapLoadedState::getName() const
{
	return "Map Loaded";
}

string MapValidatedState::getName() const
{
	return "Map Validated";
}

string PlayersAddedState::getName() const
{
	return "Players Added";
}

string AssignReinforcementState::getName() const
{
	return "Assign Reinforcement";
}

string IssueOrdersState::getName() const
{
	return "Issue Orders";
}

string ExecuteOrdersState::getName() const
{
	return "Execute Orders";
}

string WinState::getName() const
{
	return "Win";
}
