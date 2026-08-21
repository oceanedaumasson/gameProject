#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "../LoggingObserver/LoggingObserver.h"
#include "../GameEngine/GameEngine.h"

using namespace std;

class Command : public ILoggable, public Subject
{
private:
    string *commandName;
    string *commandEffect;

public:
    Command();
    Command(string commandName);
    Command(string commandName, string commandEffect);
    ~Command();

    Command(const Command &command);
    Command &operator=(const Command &command);
    friend ostream &operator<<(ostream &os, const Command &command);

    string getCommandName() const;
    string getCommandEffect() const;

    void setCommandName(string commandName);
    void saveEffect(string effect);

    string stringToLog() const override;
};

class CommandProcessor : public ILoggable, public Subject
{
private:
    const string validCommands[6] = {"loadmap", "validatemap", "addplayer", "gamestart", "replay", "quit"};

    GameEngine *gameEngine;

    vector<Command *> *commandList;

    bool validateCommandName(string commandName);
    bool validateCommandState(string commandName);

protected:
    virtual void readCommand(); // will be overriden in the fileCommandProcessor subclass
    void saveCommand(string commandName);

public:
    CommandProcessor();
    CommandProcessor(GameEngine *gameEngine);
    virtual ~CommandProcessor();

    CommandProcessor(const CommandProcessor &other);
    CommandProcessor &operator=(const CommandProcessor &other);
    friend ostream &operator<<(ostream &os, const CommandProcessor &commandProcessor);

    void getCommand();

    bool validate(Command *command);

    Command *getLatestCommand();

    void updateState(string commandState);

    string getCurrentState() const;

    string stringToLog() const override;
};

class FileLineReader
{
private:
    fstream *fileStream;
    string *filename;

public:
    FileLineReader(string filename);
    ~FileLineReader();

    FileLineReader(const FileLineReader &other);
    FileLineReader &operator=(const FileLineReader &other);
    friend ostream &operator<<(ostream &os, const FileLineReader &fileLineReader);

    string readLineFromFile();
};

class FileCommandProcessorAdapter : public CommandProcessor
{
private:
    string *fileName;
    FileLineReader *flr;

protected:
    void readCommand();

public:
    FileCommandProcessorAdapter(string fileName);
    FileCommandProcessorAdapter(string fileName, GameEngine *gameEngine);
    ~FileCommandProcessorAdapter();

    FileCommandProcessorAdapter(const FileCommandProcessorAdapter &other);
    FileCommandProcessorAdapter &operator=(const FileCommandProcessorAdapter &other);
    friend ostream &operator<<(ostream &os, const FileCommandProcessorAdapter &fileCommandProcessor);
};