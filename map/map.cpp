
#include <iostream>
#include "map.h"
#include <sstream>
#include <filesystem>

using namespace std;

// Territory method implementations

/// @brief Territory constructor
/// @param _id Id number of territory
/// @param _name Name of territory
/// @param _continentId Id of continent territory is in
Territory::Territory(int _id, const string &_name, int _continentId)
{
    id = _id;
    name = new string(_name);
    owner = nullptr;
    armyCount = 0;
    continentId = _continentId;
}
/// @brief Territory destructor
Territory::~Territory()
{
    // dont delete owner because player can own multiple territories
    owner = nullptr;
    delete (name);
    name = nullptr;
}
/// @brief Copy Contructor of territory
/// @param other Territory to copy from
Territory::Territory(const Territory &other)
{
    id = other.id;
    name = new string(other.getName());
    armyCount = other.armyCount;
    continentId = other.continentId;
    // shallow copy as player should remain the same
    owner = other.getOwner();
}
/// @brief assignment operator overload
/// @param other other territory to assign to current territory
/// @return Territory
Territory &Territory::operator=(const Territory &other)
{
    if (this == &other)
    {
        return *this; // handle self-assignment
    }

    id = other.id;
    delete (name);                      // free existing name memory
    name = new string(other.getName()); // deep copy of name
    armyCount = other.armyCount;
    continentId = other.continentId;

    owner = other.getOwner();

    return *this;
}
/// @brief stream overload for Territory
/// @param os stream to go into
/// @param territory territory to flow into stream
/// @return stream
ostream &operator<<(ostream &os, const Territory &territory)
{

    os << territory.getName() << " -- Owner: ";
    // show player if territory is owned
    if (territory.getOwner() == nullptr)
    {
        os << "not owned";
    }
    else
    {
        os << territory.getOwner()->getPlayerName();
    }

    os << " -- Army Count: " << territory.armyCount;

    return os;
}
/// @brief get the territory id number
/// @return int id
int Territory::getId() const
{
    return id;
}
/// @brief get name of territory
/// @return string name
string Territory::getName() const
{
    return *name; // dereference name pointer to return string
}
/// @brief get owner of territory
/// @return player pointer of owner, can be nullptr
Player *Territory::getOwner() const
{
    return owner;
}
/// @brief get army of territory
/// @return int army count
int Territory::getArmyCount() const
{
    return armyCount;
}
/// @brief get id of continent territory is in
/// @return in continent id
int Territory::getContinentId() const
{
    return continentId;
}
/// @brief updated player pointer of territory owner
/// @param _owner new player pointer that owns the territory
void Territory::setOwner(Player *_owner)
{
    owner = _owner;
}
/// @brief updates army count amount of territory
/// @param count new int army count to set
void Territory::setArmyCount(int count)
{
    // army count cannot be negative
    armyCount = max(0, count);
}

// Continent method implementations

/// @brief Continent constructor
/// @param _id continent id number
/// @param _name continent name string
/// @param _bonusArmyCount bonus army amount if continent is owned
Continent::Continent(int _id, const string &_name, int _bonusArmyCount)
{
    id = _id;
    name = new string(_name);
    bonusArmyCount = _bonusArmyCount;
    territories = new vector<Territory *>();
}
/// @brief continent destructor
Continent::~Continent()
{
    delete (name);
    name = nullptr;
    delete (territories);
    territories = nullptr;
}
/// @brief continent copy constructor
/// @param other continent to copy from
Continent::Continent(const Continent &other)
{
    id = other.id;
    name = new string(other.getName());
    territoryCount = other.territoryCount;
    bonusArmyCount = other.bonusArmyCount;

    territories = new vector<Territory *>();
    for (Territory *territory : *other.territories)
    {
        territories->push_back(territory);
    }
}
/// @brief continent assignment overload
/// @param other continent to assign from
/// @return Continent
Continent &Continent::operator=(const Continent &other)
{
    if (this == &other)
    {
        return *this; // handle self-assignment
    }

    id = other.id;
    delete (name); // free existing name memory
    name = new string(other.getName());
    territoryCount = other.territoryCount;
    bonusArmyCount = other.bonusArmyCount;

    delete (territories);
    territories = new vector<Territory *>();

    for (Territory *territory : *other.territories)
    {
        territories->push_back(territory);
    }

    return *this;
}
/// @brief continent stream overload
/// @param os stream to feed into
/// @param continent to print
/// @return stream
ostream &operator<<(ostream &os, const Continent &continent)
{
    os << continent.getName() << endl;

    os << "Territories:" << endl;
    for (Territory *territory : *continent.territories)
    {
        os << "\t" << *territory << endl; // use overloaded << for Territory
    }
    return os;
}
/// @brief get continent id
/// @return int id
int Continent::getId() const
{
    return id;
}
/// @brief get continent name
/// @return name string
string Continent::getName() const
{
    return *name;
}
/// @brief get army bonus count
/// @return int army bonus count
int Continent::getArmyBonusCount() const
{
    return bonusArmyCount;
}
/// @brief add territory to continent
/// @param territory to add to continent
void Continent::addTerritory(Territory *territory)
{
    territories->push_back(territory);
}
/// @brief get territories in continent
/// @return vector of territory pointers
const vector<Territory *> *Continent::getTerritories() const
{
    return territories;
}

// Map method implementations

/// @brief map constructor
/// @param _mapName name of map
Map::Map(const string &_mapName)
{
    mapName = new string(_mapName);
    territories = new vector<Territory *>();
    continents = new vector<Continent *>();
    adjacencyList = new unordered_map<Territory *, vector<Territory *> *>();
}
/// @brief map destructor
Map::~Map()
{
    for (auto &pair : *adjacencyList)
    {
        delete pair.second; // delete vector of adjacent territories
    }
    delete (adjacencyList);
    adjacencyList = nullptr;

    // delete all continent pointers
    for (Continent *continent : *continents)
    {
        delete (continent);
        continent = nullptr;
    }
    delete (continents);
    continents = nullptr;
    // delete all territory pointers
    for (Territory *territory : *territories)
    {
        delete (territory);
        territory = nullptr;
    }
    delete (territories);
    territories = nullptr;

    delete (mapName);
    mapName = nullptr;
}
/// @brief map copy constructor
/// @param other map to copy from
Map::Map(const Map &other)
{
    mapName = new string(other.getMapName());
    territories = new vector<Territory *>();
    continents = new vector<Continent *>();

    // map from old pointer address to new pointer address
    unordered_map<Territory *, Territory *> *oldToNew = new unordered_map<Territory *, Territory *>();

    // Deep copy of territories
    for (Territory *territory : *other.territories)
    {
        Territory *newTerritory = new Territory(*territory);
        territories->push_back(newTerritory);
        (*oldToNew)[territory] = newTerritory;
    }

    // territories need to be from new address not old address
    // same with territories in adjacency list
    unordered_map<Territory *, vector<Territory *> *> *newAdjacencyList = new unordered_map<Territory *, vector<Territory *> *>();
    for (auto &pair : *other.getAdjacencyList())
    {
        vector<Territory *> *newTerritories = new vector<Territory *>();

        for (Territory *territory : *(pair.second))
        {
            newTerritories->push_back((*oldToNew)[territory]);
        }

        (*newAdjacencyList)[(*oldToNew)[pair.first]] = newTerritories;
    }

    // Copy adjacency list with new territory addresses
    adjacencyList = newAdjacencyList;

    // Deep copy of continents
    for (const Continent *oldContinent : *other.continents)
    {
        // territories in continent need to be with new pointer addresses
        Continent *newContinent = new Continent(oldContinent->getId(), oldContinent->getName(), oldContinent->getArmyBonusCount());
        for (Territory *oldTerritory : *oldContinent->getTerritories())
        {
            newContinent->addTerritory((*oldToNew)[oldTerritory]);
        }
        continents->push_back(newContinent);
    }

    delete oldToNew; // clean up memory
}
/// @brief Map assignment overload
/// @param other map to assign from
/// @return Map
Map &Map::operator=(const Map &other)
{
    if (this == &other)
    {
        return *this; // handle self-assignment
    }
    delete (mapName); // free existing name memory
    mapName = new string(other.getMapName());

    for (Continent *continent : *continents)
    {
        delete (continent);
    }
    continents->clear();

    for (Territory *territory : *territories)
    {
        delete (territory); // free existing territory memory
    }
    territories->clear();

    // needs to map old territory address to new ones like in copy constructor
    unordered_map<Territory *, Territory *> *oldToNew = new unordered_map<Territory *, Territory *>();
    // Deep copy of territories
    for (Territory *territory : *other.territories)
    {
        Territory *newTerritory = new Territory(*territory);
        territories->push_back(newTerritory);
        (*oldToNew)[territory] = newTerritory;
    }

    unordered_map<Territory *, vector<Territory *> *> *newAdjacencyList = new unordered_map<Territory *, vector<Territory *> *>();
    for (auto &pair : *other.getAdjacencyList())
    {
        vector<Territory *> *newTerritories = new vector<Territory *>();

        for (Territory *territory : *(pair.second))
        {
            newTerritories->push_back((*oldToNew)[territory]);
        }

        (*newAdjacencyList)[(*oldToNew)[pair.first]] = newTerritories;
    }

    // Copy adjacency list
    for (auto &pair : *adjacencyList)
    {
        delete pair.second; // free existing adjacency list memory
    }
    delete (adjacencyList);
    adjacencyList = newAdjacencyList;

    // Deep copy of continents
    for (const Continent *oldContinent : *other.continents)
    {
        Continent *newContinent = new Continent(oldContinent->getId(), oldContinent->getName(), oldContinent->getArmyBonusCount());
        for (Territory *oldTerritory : *oldContinent->getTerritories())
        {
            newContinent->addTerritory((*oldToNew)[oldTerritory]);
        }
        continents->push_back(newContinent);
    }

    delete oldToNew;
    return *this;
}
/// @brief map stream overload
/// @param os stream to feed into
/// @param map map to print
/// @return stream
ostream &operator<<(ostream &os, const Map &map)
{
    // show map name
    os << "Map: " << map.getMapName() << endl;

    // show all continents which show all territories
    os << endl
       << "Continents:" << endl;

    for (const Continent *continent : *map.continents)
    {
        os << *continent << endl; // use overloaded << for Continent
    }

    // show borders
    os << endl
       << "Borders:" << endl;
    for (const auto &pair : *map.getAdjacencyList())
    {
        Territory *territory = pair.first;
        os << territory->getName() << " borders: " << endl;

        const vector<Territory *> &adjacentTerritories = *(pair.second);
        for (Territory *adjacentTerritory : adjacentTerritories)
        {
            os << "\t" << adjacentTerritory->getName() << endl;
        }
        os << endl;
    }

    return os;
}
/// @brief get map name
/// @return map name string
string Map::getMapName() const
{
    return *mapName;
}
/// @brief add continent to map
/// @param id of new continent
/// @param name of new continent
/// @param armyBonusCount of new continent
void Map::addContinent(int id, const string &name, int armyBonusCount)
{
    for (Continent *continent : *continents)
    {
        if (continent->getId() == id)
        {
            return; // id already exists
        }
    }
    // continent doesnt exist so create it and add to vector
    Continent *continent = new Continent(id, name, armyBonusCount);
    continents->push_back(continent);
}
/// @brief add territory to map
/// @param id of new territory
/// @param name of new territory
/// @param continentId of new territory
void Map::addTerritory(int id, const string &name, int continentId)
{

    for (Territory *territory : *territories)
    {
        if (territory->getId() == id)
        {
            return; // territory already exists
        }
    }

    Continent *continentFound = nullptr;
    for (Continent *continent : *continents)
    {
        if (continent->getId() == continentId)
        {
            continentFound = continent;
        }
    }

    if (continentFound == nullptr)
    {
        return; // continent does not exist
    }

    Territory *territory = new Territory(id, name, continentId);
    (*adjacencyList)[territory] = new vector<Territory *>(); // initialize adjacency list for the new territory
    territories->push_back(territory);                       // add territory to map
    continentFound->addTerritory(territory);                 // add territory to the corresponding continent
}
/// @brief add adjacency edge from territory1 to territory2
/// @param territory1 edge origin
/// @param territory2 edge destination
void Map::addAdjacency(Territory *territory1, Territory *territory2)
{
    bool territory1Found = false;
    bool territory2Found = false;
    for (Territory *territory : *territories)
    {
        if (territory1 == territory)
        {
            territory1Found = true;
        }
        else if (territory2 == territory)
        {
            territory2Found = true;
        }
    }

    if (!territory1Found || !territory2Found)
    {
        return; // one or both territories not found
    }

    (*adjacencyList)[territory1]->push_back(territory2); // add adjacency from territory1 to territory2
    // only need to add in this direction as it will be added in the other later when it gets to that territory
}
/// @brief get territory with passed in id
/// @param id to look for
/// @return Territory pointer of given id
Territory *Map::getTerritoryById(int id) const
{
    for (Territory *territory : *territories)
    {
        if (territory->getId() == id)
        {
            return territory; // return territory if found
        }
    }

    return nullptr; // return nullptr if territory not found
}
/// @brief get continent with passed in id
/// @param id to look for
/// @return Continent pointer of given id
Continent *Map::getContinentById(int id) const
{
    for (Continent *continent : *continents)
    {
        if (continent->getId() == id)
        {
            return continent; // return continent if found
        }
    }

    return nullptr; // return nullptr if continent not found
}
/// @brief get all territory edges
/// @return adjacency edges map
const unordered_map<Territory *, vector<Territory *> *> *Map::getAdjacencyList() const
{
    return adjacencyList;
}
/// @brief goes through all territories via dfs travel
/// @param territory origin territory
/// @param visited all visited territories
void Map::dfsVisit(Territory *territory, unordered_set<Territory *> *visited) const
{
    // insert origin territory to visited map
    visited->insert(territory);

    // get adjacency list of territory
    auto it = adjacencyList->find(territory);

    if (it != adjacencyList->end())
    {
        // go through neighbours of territory
        for (Territory *neighbour : *(it->second))
        {
            if (visited->find(neighbour) == visited->end())
            {
                // visit neighbout if not already visited
                dfsVisit(neighbour, visited);
            }
        }
    }
}
/// @brief checks if two territories are adjacent
/// @param territory1
/// @param territory2
/// @return bool if territories are adjacent
bool Map::checkAdjacent(Territory *territory1, Territory *territory2) const
{
    auto it = adjacencyList->find(territory1);
    if (it != adjacencyList->end())
    {
        const vector<Territory *> &adjacentTerritories = *(it->second);
        return find(adjacentTerritories.begin(), adjacentTerritories.end(), territory2) != adjacentTerritories.end(); // look for second territory
    }
    return false; // territory1 not found in adjacency list
}
/// @brief checks if map is connected graph
/// @return bool if map is connected graph
bool Map::isConnected() const
{
    // return false if map is empty
    if ((*territories).empty())
    {
        return false;
    }

    unordered_set<Territory *> *visited = new unordered_set<Territory *>();

    Territory *startTerritory = (*territories)[0]; // start at first territory

    dfsVisit(startTerritory, visited); // do dfs traversal through graph

    bool result = visited->size() == territories->size(); // return true if all territories were visited

    delete visited; // clean up memory
    return result;
}
/// @brief check if continent is connected subgraph
/// @param continentId to check for
/// @return bool if continent is connected subgraph
bool Map::isContinentConnected(Continent *continent) const
{
    if (continent == nullptr)
    {
        return false;
    }
    const vector<Territory *> *territories = continent->getTerritories();

    if (territories->empty())
    {
        return false;
    }

    // if continent only has 1 territory it is connected
    if (territories->size() == 1)
    {
        return true;
    }

    // territories belonging to continent
    unordered_set<Territory *> *continentSet = new unordered_set<Territory *>();
    for (Territory *territory : *territories)
    {
        continentSet->insert(territory);
    }

    unordered_set<Territory *> *visited = new unordered_set<Territory *>();
    Territory *start = (*territories)[0];

    // dfs traversal to check if it is a connected subgraph
    continentDfs(start, visited, continentSet);

    // it is connected if all territories in continent are visited
    bool connected = visited->size() == continentSet->size();

    // clear pointers
    start = nullptr;
    delete visited;
    delete continentSet;

    return connected;
}
/// @brief dfs traversal specific for a continent
/// @param territory origin to start at
/// @param visited already visited territory
/// @param continentSet territories in continent
void Map::continentDfs(Territory *territory, unordered_set<Territory *> *visited, unordered_set<Territory *> *continentSet) const
{
    visited->insert(territory);

    auto it = adjacencyList->find(territory);
    if (it != adjacencyList->end())
    {
        for (Territory *neighbour : *(it->second))
        {
            if (continentSet->find(neighbour) != continentSet->end() && visited->find(neighbour) == visited->end())
            {
                // visit neighbour if not already visited and is part of continent
                continentDfs(neighbour, visited, continentSet);
            }
        }
    }
}
/// @brief checks map is connected graph, continents are connected subgraphs, each territory belongs to only one continent
/// @return bool if all above is true
bool Map::validate() const
{
    bool isValid = true;

    if (continents->empty())
    {
        cerr << "map has no continents!" << endl;
        isValid = false;
        return isValid;
    }

    if (territories->empty())
    {
        cerr << "map has no territories!" << endl;
        isValid = false;
        return isValid;
    }

    unordered_map<int, int> territoryContinentCount;

    // check all continents are connected subgraphs
    for (Continent *continent : *continents)
    {
        if (!isContinentConnected(continent))
        {
            cerr << continent->getName() << " continent is not a connected subgraph!" << endl;
            isValid = false;
        }
        // add territory to territory continent count
        for (Territory *territory : *continent->getTerritories())
        {
            territoryContinentCount[territory->getId()]++;
        }
    }

    // check if territories belong to only one continent
    for (const Territory *territory : *territories)
    {
        int count = territoryContinentCount[territory->getId()];

        if (count == 0)
        {
            cerr << territory->getName() << " territory does not belong to a continent!" << endl;
            isValid = false;
        }
        else if (count > 1)
        {
            cerr << territory->getName() << " territory belongs to more than one continent!" << endl;
            isValid = false;
        }
    }

    // check map is a connected graph
    if (!isConnected())
    {
        cerr << getMapName() << " map is not connected!" << endl;
        isValid = false;
    }

    return isValid;
}
/// @brief get all territories of map
/// @return vector of territory pointers
vector<Territory *> *Map::getTerritories()
{
    return this->territories;
}

// MapLoader method implementations

/// @brief check if line is empty
/// @param line line to check
/// @return bool if line is empty
bool MapLoader::isWhiteSpaceLine(const string &line)
{
    return line.find_first_not_of(" \t\n\r") == string::npos;
}
/// @brief goes through all continent lines in file
/// @param inputFileStream file to parse through
/// @param map to add continents to
void MapLoader::parseContinents(ifstream &inputFileStream, Map *&map)
{
    int id = 1;
    string line;
    if (map == nullptr)
    {
        map = new Map("default_map");
    }

    while (getline(inputFileStream, line))
    {
        // Whitespace line signals end of continent section
        if (isWhiteSpaceLine(line))
        {
            break;
        }

        stringstream ss(line);
        string name;
        int bonusArmyCount;

        if (ss >> name >> bonusArmyCount)
        {
            map->addContinent(id, name, bonusArmyCount);
            id++;
        }
    }
}
/// @brief goes through all territory lines in file
/// @param inputFileStream file to parse through
/// @param map to add territories to
void MapLoader::parseTerritories(ifstream &inputFileStream, Map *&map)
{
    string line;

    while (getline(inputFileStream, line))
    {
        // Whitespace line signals end of territory section
        if (isWhiteSpaceLine(line))
        {
            break;
        }

        stringstream ss(line);
        int id;
        string name;
        int continentId;

        if (ss >> id >> name >> continentId)
        {
            map->addTerritory(id, name, continentId);
        }
    }
}
/// @brief goes through all border lines in file
/// @param inputFileStream file to parse through
/// @param map to add edges to
void MapLoader::parseBorders(ifstream &inputFileStream, Map *&map)
{
    string line;

    while (getline(inputFileStream, line))
    {
        // Whitespace line signals end of borders section
        if (isWhiteSpaceLine(line))
        {
            break;
        }

        stringstream ss(line);

        int territoryId;
        int adjacentTerritoryId;

        ss >> territoryId;

        // each territory as a list of edges in one line in the file
        while (ss >> adjacentTerritoryId)
        {
            Territory *territory = map->getTerritoryById(territoryId);
            Territory *adjacentTerritory = map->getTerritoryById(adjacentTerritoryId);

            if (territory && adjacentTerritory)
            {
                map->addAdjacency(territory, adjacentTerritory);
            }
        }
    }
}
/// @brief get map name from filename (not every file had a name in it)
/// @param filename to extract map name from
/// @return map name string
string MapLoader::extractMapName(const string &filename)
{
    filesystem::path filePath(filename);
    return filePath.stem().string(); // Get filename without extension
}
/// @brief parses through file to create a map
/// @param filename to parse through
/// @return Map pointer of created map
Map *MapLoader::loadMap(const string &filename)
{
    ifstream inputFileStream;
    inputFileStream.open(filename);

    if (!inputFileStream.is_open())
    {
        // opening failed
        return nullptr;
    }

    string mapName = extractMapName(filename);
    Map *map = new Map(mapName);

    string line;

    while (getline(inputFileStream, line))
    {

        if (line.find("[continents]") == 0)
        {
            parseContinents(inputFileStream, map);
        }
        else if (line.find("[countries]") == 0)
        {
            parseTerritories(inputFileStream, map);
        }
        else if (line.find("[borders]") == 0)
        {
            parseBorders(inputFileStream, map);
        }
    }

    inputFileStream.close();
    return map;
}
