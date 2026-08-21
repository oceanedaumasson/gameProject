/**
 * @file GameEngine.h
 * @brief Definition of the GameEngine class and the State classes.
 * This file contains the GameEngine class which manages the game flow, and the abstract
 * State class along with its concrete subclasses that represent the different phases
 * of the Warzone game.
 */

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <iostream>
#include <string>
#include <vector>
#include "../LoggingObserver/LoggingObserver.h"

// Forward declaration
class Map;
class Player;
class Deck;
class State;
class CommandProcessor;

/**
 * @brief The GameEngine class controls the flow of the game.
 * * It maintains the current state of the game and delegates commands to the
 * current state object. It also manages the transition between states.
 */
class GameEngine : public ILoggable, public Subject
{
private:
	State *currentState; /**< Pointer to the current state of the game. */
	Map *gameMap;
	std::vector<Player *> *gamePlayers;
	Deck *gameDeck;
	CommandProcessor *commandProcessor;

public:
	/**
	 * @brief Default Constructor.
	 * Initializes the game in the Start state.
	 */
	GameEngine();

	/**
	 * @brief Destructor.
	 * Cleans up the memory allocated for the current state.
	 */
	~GameEngine();

	/**
	 * @brief Copy Constructor.
	 * Creates a deep copy of the GameEngine, including its current state.
	 * @param other The GameEngine to copy.
	 */
	GameEngine(const GameEngine &other);

	/**
	 * @brief Assignment Operator.
	 * Assigns the state of one GameEngine to another.
	 * @param other The GameEngine to assign from.
	 * @return GameEngine& A reference to this object.
	 */
	GameEngine &operator=(const GameEngine &other);

	/**
	 * @brief Stream Insertion Operator.
	 * Outputs the current state of the GameEngine.
	 * @param out The output stream.
	 * @param ge The GameEngine object.
	 * @return std::ostream& The modified output stream.
	 */
	friend std::ostream &operator<<(std::ostream &out, const GameEngine &ge);

	/**
	 * @brief Game Startup.
	 * Handles map loading, validation, player creation, and the gamestart.
	 */
	void startupPhase();

	/**
	 * @brief Transitions the engine to a new state.
	 * @param newState Pointer to the new State object.
	 */
	void transition(State *newState);

	/**
	 * @brief Processes a command from the user.
	 * Forwards the command to the current state's updateState method.
	 * @param command The command string.
	 */
	void handleCommand(std::string command);

	/**
	 * @brief Gets the current state object.
	 * @return State* Pointer to the current state.
	 */
	State *getCurrentState();

	/**
	 * @brief Sets the game map
	 * @param m Pointer to the Map object
	 */
	void setMap(Map *m);

	/**
	 * @brief Sets the list of players
	 * @param players Pointer to a vector of Player pointers
	 */
	void setPlayers(std::vector<Player *> *players);

	/**
	 * @brief Gets the game map
	 * @return Map* Pointer to the Map object
	 */
	Map *getMap() const;

	/**
	 * @brief Gets the list of players
	 * @return std::vector<Player*>* Pointer to the vector of Player pointers
	 */
	std::vector<Player *> *getPlayers() const;

	/**
	 * @brief Runs the main game loop until a player wins
	 * Loops through reinforcement, issue orders and execute orders phases
	 */
	void mainGameLoop();

	/**
	 * @brief Gives armies to each players based on territories owned
	 * Gives continent bonus if player owns all a continent's territories
	 */
	void reinforcementPhase();

	/**
	 * @brief Each player issues orders in round-robin fashion
	 */
	void issueOrdersPhase();

	/**
	 * @brief Executes all player orders in round-robin fashion
	 */
	void executeOrdersPhase();

	string stringToLog() const override;
};

/**
 * @brief Abstract Base Class for the State.
 * * This class defines the interface that all specific game states must implement.
 */
class State
{
public:
	/**
	 * @brief Default Constructor.
	 */
	State();

	/**
	 * @brief Destructor.
	 * Virtual to ensure correct cleanup of derived classes.
	 */
	virtual ~State();

	/**
	 * @brief Copy Constructor.
	 * @param other The State object to copy from.
	 */
	State(const State &other);

	/**
	 * @brief Assignment Operator.
	 * @param other The State object to assign from.
	 * @return State& A reference to this object.
	 */
	State &operator=(const State &other);

	/**
	 * @brief Stream Insertion Operator.
	 * @param out The output stream.
	 * @param s The State object to output.
	 * @return std::ostream& The modified output stream.
	 */
	friend std::ostream &operator<<(std::ostream &out, const State &s);

	/**
	 * @brief Updates the game state based on the user command.
	 * Pure virtual function that must be implemented by concrete states.
	 * @param ge Pointer to the GameEngine.
	 * @param command The user command string.
	 */
	virtual void updateState(GameEngine *ge, std::string command) = 0;

	/**
	 * @brief Gets the name of the state.
	 * @return std::string The name of the state.
	 */
	virtual std::string getName() const = 0;
};

/**
 * @brief Represents the start state of the game.
 * Valid transition: "loadmap" -> MapLoadedState.
 */
class StartState : public State
{
public:
	StartState();
	StartState(const StartState &other);
	StartState &operator=(const StartState &other);
	friend std::ostream &operator<<(std::ostream &out, const StartState &s);
	void updateState(GameEngine *ge, std::string command) override;
	std::string getName() const override;
};

/**
 * @brief Represents the state where a map has been loaded.
 * Valid transitions: "loadmap", "validatemap" -> MapValidatedState.
 */
class MapLoadedState : public State
{
public:
	MapLoadedState();
	MapLoadedState(const MapLoadedState &other);
	MapLoadedState &operator=(const MapLoadedState &other);
	friend std::ostream &operator<<(std::ostream &out, const MapLoadedState &s);
	void updateState(GameEngine *ge, std::string command) override;
	std::string getName() const override;
};

/**
 * @brief Represents the state where the map has been validated.
 * Valid transition: "addplayer" -> PlayersAddedState.
 */
class MapValidatedState : public State
{
public:
	MapValidatedState();
	MapValidatedState(const MapValidatedState &other);
	MapValidatedState &operator=(const MapValidatedState &other);
	friend std::ostream &operator<<(std::ostream &out, const MapValidatedState &s);
	void updateState(GameEngine *ge, std::string command) override;
	std::string getName() const override;
};

/**
 * @brief Represents the state where players are being added to the game.
 * Valid transitions: "addplayer", "assigncountries" -> AssignReinforcementState.
 */
class PlayersAddedState : public State
{
public:
	PlayersAddedState();
	PlayersAddedState(const PlayersAddedState &other);
	PlayersAddedState &operator=(const PlayersAddedState &other);
	friend std::ostream &operator<<(std::ostream &out, const PlayersAddedState &s);
	void updateState(GameEngine *ge, std::string command) override;
	std::string getName() const override;
};

/**
 * @brief Represents the start of a turn where reinforcements are assigned.
 * Valid transition: "issueorder" -> IssueOrdersState.
 */
class AssignReinforcementState : public State
{
public:
	AssignReinforcementState();
	AssignReinforcementState(const AssignReinforcementState &other);
	AssignReinforcementState &operator=(const AssignReinforcementState &other);
	friend std::ostream &operator<<(std::ostream &out, const AssignReinforcementState &s);
	void updateState(GameEngine *ge, std::string command) override;
	std::string getName() const override;
};

/**
 * @brief Represents the phase where players issue orders.
 * Valid transitions: "issueorder", "endissueorders" -> ExecuteOrdersState.
 */
class IssueOrdersState : public State
{
public:
	IssueOrdersState();
	IssueOrdersState(const IssueOrdersState &other);
	IssueOrdersState &operator=(const IssueOrdersState &other);
	friend std::ostream &operator<<(std::ostream &out, const IssueOrdersState &s);
	void updateState(GameEngine *ge, std::string command) override;
	std::string getName() const override;
};

/**
 * @brief Represents the phase where orders are executed.
 * Valid transitions: "execorder", "endexecorders" -> AssignReinforcementState, "win" -> WinState.
 */
class ExecuteOrdersState : public State
{
public:
	ExecuteOrdersState();
	ExecuteOrdersState(const ExecuteOrdersState &other);
	ExecuteOrdersState &operator=(const ExecuteOrdersState &other);
	friend std::ostream &operator<<(std::ostream &out, const ExecuteOrdersState &s);
	void updateState(GameEngine *ge, std::string command) override;
	std::string getName() const override;
};

/**
 * @brief Represents the end of the game where a player has won.
 * Valid transitions: "play" -> StartState (Restart), "end" -> Exit Game.
 */
class WinState : public State
{
public:
	WinState();
	WinState(const WinState &other);
	WinState &operator=(const WinState &other);
	friend std::ostream &operator<<(std::ostream &out, const WinState &s);
	void updateState(GameEngine *ge, std::string command) override;
	std::string getName() const override;
};

#endif