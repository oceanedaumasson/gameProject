#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <algorithm>
#include "../Player/Player.h"

using namespace std;

class Territory
{
private:
    int id;
    string *name;

    Player *owner;
    int armyCount;
    int continentId;

public:
    Territory(int _id, const string &_name, int _continentId);
    ~Territory();

    Territory(const Territory &other);
    Territory &operator=(const Territory &other);
    friend ostream &operator<<(ostream &os, const Territory &territory);

    int getId() const;
    string getName() const;
    Player *getOwner() const;
    int getArmyCount() const;
    int getContinentId() const;
    void setOwner(Player *_owner);
    void setArmyCount(int count);
};

class Continent
{
private:
    int id;
    string *name;
    int territoryCount;
    int bonusArmyCount;
    vector<Territory *> *territories;

public:
    Continent(int _id, const string &_name, int _bonusArmyCount);
    ~Continent();
    Continent(const Continent &other);
    Continent &operator=(const Continent &other);
    friend ostream &operator<<(ostream &os, const Continent &continent);

    int getId() const;
    string getName() const;
    int getArmyBonusCount() const;
    void addTerritory(Territory *territory);
    const vector<Territory *> *getTerritories() const;
};

class Map
{
private:
    string *mapName;
    vector<Territory *> *territories;
    vector<Continent *> *continents;
    unordered_map<Territory *, vector<Territory *> *> *adjacencyList;
    void dfsVisit(Territory *territory, unordered_set<Territory *> *visited) const;
    void continentDfs(Territory *territory, unordered_set<Territory *> *visited, unordered_set<Territory *> *continentSet) const;
    bool isConnected() const;
    bool isContinentConnected(Continent *continent) const;

public:
    Map(const string &_mapName);
    ~Map();

    Map(const Map &other);
    Map &operator=(const Map &other);
    friend ostream &operator<<(ostream &os, const Map &map);

    string getMapName() const;
    void addContinent(int id, const string &name, int armyBonusCount);
    void addTerritory(int id, const string &name, int continentId);
    void addAdjacency(Territory *territory1, Territory *territory2);
    vector<Territory *> *getTerritories();
    Territory *getTerritoryById(int id) const;
    Continent *getContinentById(int id) const;
    const unordered_map<Territory *, vector<Territory *> *> *getAdjacencyList() const;
    bool checkAdjacent(Territory *, Territory *) const;
    bool validate() const;
};

class MapLoader
{
private:
    void parseContinents(ifstream &inputFileStream, Map *&map);
    void parseTerritories(ifstream &inputFileStream, Map *&map);
    void parseBorders(ifstream &inputFileStream, Map *&map);
    bool isWhiteSpaceLine(const string &line);
    string extractMapName(const string &filename);

public:
    MapLoader() = default;
    ~MapLoader() = default;

    Map *loadMap(const string &filename);
};
